using OpenZWave;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
#if NETFX_CORE
using Windows.Devices.Enumeration;
using Windows.UI.Core;
#else
using CoreDispatcher = System.Windows.Threading.Dispatcher;
#endif

namespace OpenZWave.NetworkManager
{
    /// <summary>
    /// This class monitors discovery, removal and changes to available devices (nodes),
    /// and raises the proper change notifications to use it for binding to a view
    /// </summary>
    public class Watcher : INotifyPropertyChanged
    {
        private readonly List<ZWaveRequest> m_pendingRequests = new List<ZWaveRequest>();
        private readonly ObservableCollection<Node> m_nodeList = new ObservableCollection<Node>();
        private readonly ObservableCollection<Controller> m_controllerList = new ObservableCollection<Controller>();
        public static Watcher Instance { get; private set; }

        private readonly CoreDispatcher Dispatcher;

        /// <summary>
        /// Initializes a new instance of the <see cref="Watcher"/> class
        /// </summary>
        /// <param name="dispatcher">Dispatcher to use for raising change notification.
        /// If null, change notificatin events will can be raised on any thread. If you are relying on binding,
        /// make sure you assign the UI Thread Dispatcher here</param>
        public Watcher(CoreDispatcher dispatcher = null)
        {
            if (Instance != null)
                throw new InvalidOperationException("Only one watcher instance can be created");
            Dispatcher = dispatcher;
            Instance = this;            
        }

        public void Initialize(string configPath = "/config", string userPath = null, string commandLine = "")
        {
            if (userPath == null)
            {
#if NETFX_CORE
                userPath = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
#else
	            userPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
#endif
            }
            ZWOptions.Instance.Initialize(configPath, userPath, commandLine);

            if (!ZWOptions.Instance.AreLocked)
            {
                // Add any app specific options here...

                // ordinarily, just write "Detail" level messages to the log
                //ZWOptions.Instance.AddOptionInt("SaveLogLevel", (int)ZWLogLevel.Detail);

                // save recent messages with "Debug" level messages to be dumped if an error occurs
                //ZWOptions.Instance.AddOptionInt("QueueLogLevel", (int)ZWLogLevel.Debug);

                // only "dump" Debug to the log emessages when an error-level message is logged
                //ZWOptions.Instance.AddOptionInt("DumpTriggerLevel", (int)ZWLogLevel.Error);

                // Lock the options
                ZWOptions.Instance.Lock();
            }
            // Create the OpenZWave Manager
            ZWManager.Instance.Initialize();
            ZWManager.Instance.NotificationReceived += OnNodeNotification;
        }

        public void AddController(string portId, string displayName)
        {
            m_controllerList.Add(new Controller(portId, displayName));
            ZWManager.Instance.AddDriver(portId);
        }

        public void RemoveController(string portId)
        {
            ZWManager.Instance.RemoveDriver(portId);
            // m_controllerList will automatically be updated when the DriverRemoved notification triggers
        }

        /// <summary>
        /// Gets a list of the available/discovered nodes
        /// </summary>
        public IEnumerable<Node> Nodes => m_nodeList;

        /// <summary>
        /// Gets a list of controllers registered with the controller.
        /// </summary>
        public IEnumerable<Controller> Controllers => m_controllerList;

        /// <summary>
        /// The notifications handler.
        /// </summary>
        /// <param name="notification">The notification.</param>
        private void OnNodeNotification(ZWManager manager, NotificationReceivedEventArgs e)
        {
            if (Dispatcher == null)
            {
                NotificationHandler(e.Notification);
            }
            else
            {
                // Handle the notification on a thread that can safely
                // modify the controls without throwing an exception.
#if NETFX_CORE
                var _ = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
#else
            Dispatcher.BeginInvoke(new Action(() =>
#endif
                {
                    NotificationHandler(e.Notification);
                }
#if !NETFX_CORE
            )
#endif
            );
            }
        }

        private class ZWaveRequest
        {
            private DateTimeOffset creationTime;
            public ZWaveRequest()
            {
                creationTime = DateTimeOffset.Now;
            }
            public TimeSpan Age { get { return DateTimeOffset.Now - creationTime; } }
            public ZWNotificationType Type { get; set; }
            public byte HomeID { get; set; }
            public byte NodeID { get; set; }
            public TaskCompletionSource<ZWNotification> CompletionSource { get; set; }
            public TimeSpan Timeout { get; set; } = TimeSpan.MaxValue;
        }

        /// <summary>
        /// The notification handler. This is called by the ZWave library every time
        /// an event occurs on the ZWave network, a device is found/lost, etc
        /// </summary>
        private void NotificationHandler(ZWNotification notification)
        {
            var value = notification.ValueId;
            
            var homeID = notification.HomeId;
            var nodeId = notification.NodeId;
            var type = notification.Type;

            Action<ZWValueId> debugWriteValueID = (v) =>
            {
                // Debug.WriteLine("  Node : " + nodeId.ToString());
                // Debug.WriteLine("  CC   : " + v.CommandClassId.ToString());
                // Debug.WriteLine("  Type : " + type.ToString());
                // Debug.WriteLine("  Index: " + v.Index.ToString());
                // Debug.WriteLine("  Inst : " + v.Instance.ToString());
                // Debug.WriteLine("  Value: " + GetValue(v).ToString());
                // Debug.WriteLine("  Label: " + m_manager.GetValueLabel(v));
                // Debug.WriteLine("  Help : " + m_manager.GetValueHelp(v));
                // Debug.WriteLine("  Units: " + m_manager.GetValueUnits(v));
            };

            foreach(var item in m_pendingRequests.ToArray())
            {
                if(item.HomeID == homeID && (item.NodeID == nodeId || item.NodeID < 0) && item.Type == type)
                {
                    item.CompletionSource.SetResult(notification);
                    m_pendingRequests.Remove(item);
                }
                else if(item.Age > item.Timeout)
                {
                    item.CompletionSource.SetException(new TimeoutException());
                    m_pendingRequests.Remove(item);
                }
            }

            switch (notification.Type)
            {
                // NodeAdded : Node now exists in the system. Very little useful info
                case ZWNotificationType.NodeAdded:
                    {
                        // if this node was in zwcfg*.xml, this is the first node notification
                        // if not, the NodeNew notification should already have been received
                        if (GetNode(homeID, nodeId) == null)
                        {
                            m_nodeList.Add(new Node(nodeId, homeID));
                        }
                        break;
                    }

                case ZWNotificationType.NodeNew:
                    {
                        // Add the new node to our list (and flag as uninitialized)
                        m_nodeList.Add(new Node(nodeId, homeID));
                        break;
                    }

                case ZWNotificationType.NodeRemoved:
                    {
                        foreach (Node node in m_nodeList)
                        {
                            if (node.ID == nodeId)
                            {
                                m_nodeList.Remove(node);
                                node.RaiseNodeRemoved();
                                break;
                            }
                        }
                        break;
                    }

                case ZWNotificationType.NodeProtocolInfo:
                case ZWNotificationType.NodeEvent:
                case ZWNotificationType.NodeNaming:
                case ZWNotificationType.EssentialNodeQueriesComplete:
                case ZWNotificationType.NodeQueriesComplete:
                case ZWNotificationType.ValueRemoved:
                case ZWNotificationType.ValueChanged:
                case ZWNotificationType.Group:
                {
                        Node node = GetNode(homeID, nodeId);
                        if (node != null)
                        {
                            node.HandleNodeEvent(notification);
                        }
                        break;
                    }

                case ZWNotificationType.PollingDisabled:
                    {
                        Debug.WriteLine("Polling disabled notification");
                        break;
                    }

                case ZWNotificationType.PollingEnabled:
                    {
                        Debug.WriteLine("Polling enabled notification");
                        break;
                    }

                case ZWNotificationType.DriverReady:
                    {
                        var p = ZWManager.Instance.GetControllerPath(homeID);
                        var ctrl = Controllers.Where(c => c.ControllerPath == p).FirstOrDefault();
                        if (ctrl != null)
                        {
                            ctrl.HandleControllerEvent(notification);
                        }
                        else
                        {
                            m_controllerList.Add(new Controller(p, p, homeID));
                        }
                        break;
                    }
                case ZWNotificationType.DriverFailed:
                    {
                        // TODO: if HomeID is 0 we don't really know anything about which one failed
                        Debug.WriteLine("Driver failed for HomeID " + homeID.ToString());
                        var d = Controllers.Where(t => t.HomeId == homeID).FirstOrDefault();
                        if (d != null)
                        {
                            d.UpdateDriverStatus(DriverStatus.Failed);
                        }
                        break;
                    }
                case ZWNotificationType.DriverRemoved:
                    {
                        var d = Controllers.Where(t => t.HomeId == homeID).FirstOrDefault();
                        var nodes = GetNodes(homeID).ToArray();
                        foreach (var node in nodes)
                            m_nodeList.Remove(node);
                        if (d != null)
                        {
                            m_controllerList.Remove(d);
                        }
                        break;
                    }
                case ZWNotificationType.AllNodesQueried:
                case ZWNotificationType.AllNodesQueriedSomeDead:
                case ZWNotificationType.AwakeNodesQueried:
                    {
                        var d = Controllers.Where(t => t.HomeId == homeID).FirstOrDefault();
                        d?.HandleControllerEvent(notification);
                        break;
                    }

                default:
                    {
                        if (nodeId > 0)
                        {
                            Node node = GetNode(homeID, nodeId);
                            if (node != null)
                            {
                                node.HandleNodeEvent(notification);
                            }
                        }
                        else
                        {
                            //var v = GetValue(notification.ValueID);
                            Debug.WriteLine($"******Controller error '{notification.Code}'");
                            //Debug.WriteLine($"******Notification '{type}' not Handled @ ID: {nodeId}");
                        }
                        break;
                    }
            }
        }
        
        /// <summary>
        /// Gets the node.
        /// </summary>
        /// <param name="homeId">The home identifier.</param>
        /// <param name="nodeId">The node identifier.</param>
        /// <returns></returns>
        private Node GetNode(UInt32 homeId, Byte nodeId)
        {
            foreach (Node node in m_nodeList)
            {
                if ((node.ID == nodeId) && (node.HomeID == homeId))
                {
                    return node;
                }
            }

            return null;
        }

        private IEnumerable<Node> GetNodes(UInt32 homeId)
        {
            foreach (Node node in m_nodeList)
            {
                if (node.HomeID == homeId)
                {
                    yield return node;
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
