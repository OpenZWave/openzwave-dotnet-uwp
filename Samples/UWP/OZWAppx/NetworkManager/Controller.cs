using OpenZWave;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OpenZWave.NetworkManager
{
    public enum DriverStatus
    {
        Loading,
        DriverReady,
        AwakeNodesQueried,
        AllNodesQueriedSomeDead,
        AllNodesQueried,
        Failed,
    }

    public class Controller : INotifyPropertyChanged
    {
        private string _path;

        internal Controller(string path, string displayName, uint homeId = 0)
        {
            _path = path;
            HomeId = homeId;
            if (homeId == 0)
                DriverStatus = DriverStatus.Loading;
            else
                DriverStatus = DriverStatus.DriverReady;
            DisplayName = displayName ?? path;
        }

        public string DisplayName { get; }

        internal void UpdateHomeId(uint homeid)
        {
            HomeId = homeid;
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(HomeId)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsPrimary)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsStaticUpdateController)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsBridgeController)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LibraryVersion)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LibraryTypeName)));
            UpdateDriverStatus(DriverStatus.DriverReady);
            DriverReady?.Invoke(this, EventArgs.Empty);
        }

        public uint HomeId { get; private set; }

        /// <summary>
        /// Get the node ID of the Z-Wave controller.
        /// </summary>
        /// <value>The node ID of the Z-Wave controller</value>
        public uint NodeId => HomeId == 0 ? 0u : ZWManager.Instance.GetControllerNodeId(HomeId);

        public DriverStatus DriverStatus { get; private set; }

        internal void UpdateDriverStatus(DriverStatus status)
        {
            DriverStatus = status;
            switch (status)
            {
                case DriverStatus.AllNodesQueried:
                    AllNodesQueried?.Invoke(this, EventArgs.Empty);
                    break;
                case DriverStatus.AwakeNodesQueried:
                    AllAwakeNodesQueried?.Invoke(this, EventArgs.Empty);
                    break;
                case DriverStatus.Failed:
                    DriverFailed?.Invoke(this, EventArgs.Empty);
                    break;
                case DriverStatus.AllNodesQueriedSomeDead:
                default:
                    break;
            }
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(DriverStatus)));
        }

        /// <summary>
        /// Get count of messages in the outgoing send queue. 
        /// </summary>
        /// <value>An integer message count</value>
        public int SendQueueCount => HomeId == 0 ? 0 : ZWManager.Instance.GetSendQueueCount(HomeId);

        /// <summary>
        /// True if the controller is a primary controller. The primary controller is the main device 
        /// used to configure and control a Z-Wave network. There can only be one primary controller - 
        /// all other controllers are secondary controllers.
        /// </summary>
        public bool IsPrimary => HomeId == 0 ? false : ZWManager.Instance.IsPrimaryController(HomeId);

        /// <summary>
        /// True if the controller is a static update controller. A Static Update Controller (SUC) is a 
        /// controller that must never be moved in normal operation and which can be used by other nodes
        /// to receive information about network changes.
        /// </summary>
        public bool IsStaticUpdateController => HomeId == 0 ? false : ZWManager.Instance.IsStaticUpdateController(HomeId);

        /// <summary>
        /// Query if the controller is using the bridge controller library. A bridge controller is able 
        /// to create virtual nodes that can be associated with other controllers to enable events to be
        /// passed on. 
        /// </summary>
        public bool IsBridgeController => HomeId == 0 ? false : ZWManager.Instance.IsBridgeController(HomeId);

        /// <summary>
        /// Obtains the controller interface path. 
        /// </summary>
        public string ControllerPath => HomeId == 0 ? _path : ZWManager.Instance.GetControllerPath(HomeId);

        /// <summary>
        /// Get the version of the Z-Wave API library used by a controller.
        /// </summary>
        /// <value>A string containing the library version. For example, "Z-Wave 2.48". </value>
        public string LibraryVersion => HomeId == 0 ? null : ZWManager.Instance.GetLibraryVersion(HomeId);

        /// <summary>
        /// Get a string containing the Z-Wave API library type used by a controller. The possible library types are: 
        /// Static Controller, Controller, Enhanced Slave, Slave, Installer, Routing Slave, Bridge Controller, Device Under Test.
        /// The controller should never return a slave library type. For a more efficient test of whether a controller 
        /// is a Bridge Controller, use the <see cref="IsBridgeController"/> property. 
        /// </summary>
        public string LibraryTypeName => HomeId == 0 ? null : ZWManager.Instance.GetLibraryTypeName(HomeId);

        /// <summary>
        /// Receive network configuration information from primary controller. Requires secondary.
        /// </summary>
        /// <remarks>
        /// This command prepares the controller to recieve Network Configuration from a
        /// Secondary Controller.
        /// Results of the ReceiveConfiguration Command will be sent as a Notification with
        /// the Notification type as Notification::Type_ControllerCommand
        /// </remarks>
        /// <returns><c>true</c>if the ReceiveConfiguration Command was sent succesfully to the Controller</returns>
        public bool ReceiveConfiguration() => ZWManager.Instance.ReceiveConfiguration(HomeId);

        /// <summary>
        /// Start the Inclusion Process to add a Node to the Network.
        /// </summary>
        /// <param name="doSecurity">Whether to initialize the Network Key on the device if it supports the Security CC</param>
        /// <returns>True if the Command was sent succcesfully to the Controller</returns>
        public bool AddNode(bool doSecurity) => ZWManager.Instance.AddNode(HomeId, doSecurity);

        /// <summary>
        /// Remove a Device from the Z-Wave Network
        /// </summary>
        /// <remarks>
        /// The Status of the Node Removal is communicated via Notifications. Specifically,
        /// you should monitor ControllerCommand Notifications.
        /// Results of the AddNode Command will be send as a Notification with the Notification
        /// type as Notification::Type_ControllerCommand
        /// </remarks>
        /// <returns>True if the Command was sent succesfully to the Controller</returns>
        public bool RemoveNode() => ZWManager.Instance.RemoveNode(HomeId);

        /// <summary>
        /// Soft Reset a PC Z-Wave Controller.
        /// </summary>
        /// <remarks>Resets a controller without erasing its network configuration settings.</remarks>
        public void SoftReset() => ZWManager.Instance.SoftReset(HomeId);

        /// <summary>
        /// Hard Reset a PC Z-Wave Controller.
        /// </summary>
        /// <remarks>
        /// Resets a controller and erases its network configuration settings. The controller
        /// becomes a primary controller ready to add devices to a new network.
        /// </remarks>
        public void ResetController() => ZWManager.Instance.ResetController(HomeId);

        /// <summary>
        /// Create a new primary controller when old primary fails. Requires a secondary update controller (SUC).
        /// </summary>
        /// <remarks>
        /// This command Creates a new Primary Controller when the Old Primary has Failed.
        /// Requires a SUC on the network to function
        /// Results of the CreateNewPrimary Command will be send as a Notification with the
        /// Notification type as Notification::Type_ControllerCommand
        /// </remarks>
        /// <returns>True if the CreateNewPrimary Command was sent succesfully to the Controller</returns>
        public bool CreateNewPrimary() => ZWManager.Instance.CreateNewPrimary(HomeId);

        /// <summary>
        /// Add a new controller to the network and make it the primary.
        /// </summary>
        /// <remarks>
        /// The existing primary will become a secondary controller.
        /// Results of the TransferPrimaryRole Command will be send as a Notification with
        /// the Notification type as Notification::Type_ControllerCommand
        /// </remarks>
        /// <returns>True if the TransferPrimaryRole Command was send succesfully to the Controller</returns>
        public bool TransferPrimaryRole() => ZWManager.Instance.TransferPrimaryRole(HomeId);

        /// <summary>
        ///  A driver will handle the sending and receiving of messages for all the devices in its controller's Z-Wave 
        ///  network. The Driver will read any previously saved configuration and then query the Z-Wave controller
        ///  for any missing information. Once that process is complete, a DriverReady notification callback will 
        ///  be sent containing the Home ID of the controller, which is required by most of the other Manager class 
        ///  methods.
        ///  The event argument will contain the Home ID.
        /// </summary>
        /// <seealso cref="Controllers"/>
        public event EventHandler DriverReady;

        /// <summary>
        /// Driver failed to load
        /// </summary>
        public event EventHandler DriverFailed;

        /// <summary>
        /// After all "awake" nodes have been polled, an "AllAwakeNodesQueried" notification is sent.
        /// This is when a client application can expect all of the node information (both static information,
        /// like the physical device's capabilities, session information (like [associations and/or names] and
        /// dynamic information (like temperature or on/off state) to be available
        /// The event argument will contain the Home ID.
        /// </summary>
        /// <seealso cref="AllNodesQueried"/>
        /// <seealso cref="DriverReady"/>
        public event EventHandler AllAwakeNodesQueried;

        /// <summary>
        /// After all nodes (whether listening or sleeping) have been polled, an "AllNodesQueried" notification is sent
        /// The event argument will contain the Home ID.
        /// </summary>
        /// <seealso cref="AllAwakeNodesQueried"/>
        public event EventHandler AllNodesQueried;

        public event PropertyChangedEventHandler PropertyChanged;



        /// <summary>
        /// Called by the <see cref="Watcher"/> when events specific to this instance occur
        /// </summary>
        /// <param name="notification"></param>
        internal bool HandleControllerEvent(ZWNotification notification)
        {
            var homeID = notification.HomeId;
            var nodeId = notification.NodeId;
            var type = notification.Type;

            switch (type)
            {
                case ZWNotificationType.DriverReady:
                    {
                        UpdateHomeId(homeID);
                        return true;
                    }
                case ZWNotificationType.AllNodesQueried:
                    {
                        Debug.WriteLine("All nodes queried");
                        UpdateDriverStatus(DriverStatus.AllNodesQueried);
                        return true;
                    }

                case ZWNotificationType.AllNodesQueriedSomeDead:
                    {
                        Debug.WriteLine("All nodes queried (some dead)");
                        UpdateDriverStatus(DriverStatus.AllNodesQueriedSomeDead);
                        return true;
                    }

                case ZWNotificationType.AwakeNodesQueried:
                    {
                        UpdateDriverStatus(DriverStatus.AwakeNodesQueried);
                        return true;
                    }
                default:
                    return false;
            }
        }
    }
}
