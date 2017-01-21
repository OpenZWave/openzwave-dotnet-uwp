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
using Windows.Devices.Enumeration;
#if NETFX_CORE
using Windows.UI.Core;
#else
using CoreDispatcher = System.Windows.Threading.Dispatcher;
#endif

namespace OZWAppx
{
    public class MainViewModel : INotifyPropertyChanged
    {
        public static MainViewModel Instance { get; private set; }

        private CoreDispatcher Dispatcher { get; }
        
        public MainViewModel(CoreDispatcher dispatcher)
        {
            if (Instance != null)
                throw new InvalidOperationException("Only one manager instance can be created");
            Dispatcher = dispatcher;
            Instance = this;            
        }

        public async Task Initialize()
        {
#if NETFX_CORE
            var userPath = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
#else
            var userPath = "";
#endif
            ZWOptions.Instance.Initialize();

            // Add any app specific options here...

            // ordinarily, just write "Detail" level messages to the log
            //m_options.AddOptionInt("SaveLogLevel", (int)ZWLogLevel.Detail);

            // save recent messages with "Debug" level messages to be dumped if an error occurs
            //m_options.AddOptionInt("QueueLogLevel", (int)ZWLogLevel.Debug);

            // only "dump" Debug  to the log emessages when an error-level message is logged
            //m_options.AddOptionInt("DumpTriggerLevel", (int)ZWLogLevel.Error);

            // Lock the options
            ZWOptions.Instance.Lock();

            // Create the OpenZWave Manager
            ZWManager.Instance.Initialize();
            ZWManager.Instance.OnNotification += OnNodeNotification;

            var serialPortSelector = Windows.Devices.SerialCommunication.SerialDevice.GetDeviceSelector();
            var devices = await DeviceInformation.FindAllAsync(serialPortSelector);
            foreach (var item in devices)
            {
                SerialPorts.Add(new SerialPortInfo(item));
            }
        }
        
        public ObservableCollection<SerialPortInfo> SerialPorts { get; } = new ObservableCollection<SerialPortInfo>();

        public class SerialPortInfo
        {
            public SerialPortInfo(DeviceInformation info)
            {
                PortID = info.Id;
                Name = info.Name;
            }
            public string PortID { get; }
            public string Name { get; }
            private bool _isActive;

            public bool IsActive
            {
                get { return _isActive; }
                set {
                    _isActive = value;
                    if (value)
                        ZWManager.Instance.AddDriver(PortID);
                    else
                        ZWManager.Instance.RemoveDriver(PortID);
                }
            }
        }

        /// <summary>
        /// The notifications handler.
        /// </summary>
        /// <param name="notification">The notification.</param>
        public void OnNodeNotification(ZWNotification notification)
        {
            // Handle the notification on a thread that can safely
            // modify the controls without throwing an exception.
#if NETFX_CORE
            var _ = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
#else
            Dispatcher.BeginInvoke(new Action(() =>
#endif
            {
                NotificationHandler(notification);
            }
#if !NETFX_CORE
            )
#endif
            );
        }

        private List<ZWaveRequest> PendingRequests = new List<ZWaveRequest>();

        private class ZWaveRequest
        {
            private DateTimeOffset creationTime;
            public ZWaveRequest()
            {
                creationTime = DateTimeOffset.Now;
            }
            public TimeSpan Age { get { return DateTimeOffset.Now - creationTime; } }
            public NotificationType Type { get; set; }
            public byte HomeID { get; set; }
            public byte NodeID { get; set; }
            public TaskCompletionSource<ZWNotification> TCS { get; set; }
            public TimeSpan Timeout { get; set; } = TimeSpan.MaxValue;
        }

        /// <summary>
        /// The notification handler.
        /// </summary>
        private void NotificationHandler(ZWNotification notification)
        {
            var homeID = notification.HomeId;
            var nodeId = notification.NodeId;
            var type = notification.Type;

            Action<ZWValueID> debugWriteValueID = (v) =>
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

            foreach(var item in PendingRequests.ToArray())
            {
                if(item.HomeID == homeID && (item.NodeID == nodeId || item.NodeID < 0) && item.Type == type)
                {
                    item.TCS.SetResult(notification);
                    PendingRequests.Remove(item);
                }
                else if(item.Age > item.Timeout)
                {
                    item.TCS.SetException(new TimeoutException());
                    PendingRequests.Remove(item);
                }
            }
            // NodeQueriesComplete : When all nodes has reported back
            // NodeAdded : Node now exists in the system. Very little useful info
            // NodeProtocolInfo: We now know what type of node it is
            // Debug.WriteLine($"Notification Received: {homeID}:{nodeId} = {type}");
            switch (notification.Type)
            {
                case NotificationType.ValueAdded:
                    {
                        Node node = GetNode(homeID, nodeId);
                        if (node != null)
                        {
                            var value = notification.ValueID;
                            node.AddValue(value);
                            debugWriteValueID(value);
                        }
                        break;
                    }

                case NotificationType.ValueRemoved:
                    {
                        Node node = GetNode(homeID, nodeId);
                        if (node != null)
                        {
                            node.RemoveValue(notification.ValueID);
                        }
                        break;
                    }

                case NotificationType.ValueChanged:
                    {
                        Debug.WriteLine("Value Changed");
                        ZWValueID value = notification.ValueID;
                        debugWriteValueID(value);

                        Node node = GetNode(homeID, nodeId);
                        if (node != null)
                        {
                            node.AddValue(value);
                        }
                        break;
                    }

                case NotificationType.Group:
                    {
                        break;
                    }

                case NotificationType.NodeAdded:
                    {
                        // if this node was in zwcfg*.xml, this is the first node notification
                        // if not, the NodeNew notification should already have been received
                        if (GetNode(homeID, nodeId) == null)
                        {
                            m_nodeList.Add(new Node(nodeId, homeID));
                        }
                        break;
                    }

                case NotificationType.NodeNew:
                    {
                        // Add the new node to our list (and flag as uninitialized)
                        m_nodeList.Add(new Node(nodeId, homeID));
                        break;
                    }

                case NotificationType.NodeRemoved:
                    {
                        foreach (Node node in m_nodeList)
                        {
                            if (node.ID == nodeId)
                            {
                                m_nodeList.Remove(node);
                                break;
                            }
                        }
                        break;
                    }

                case NotificationType.NodeProtocolInfo:
                    {
                        Node node = GetNode(homeID, nodeId);
                        if (node != null)
                        {
                            node.Label = ZWManager.Instance.GetNodeType(homeID, node.ID);
                            node.IsLoading = false;
                        }
                        break;
                    }

                case NotificationType.NodeNaming:
                    {
                        Node node = GetNode(homeID, nodeId);
                        if (node != null)
                        {
                            node.Manufacturer = ZWManager.Instance.GetNodeManufacturerName(homeID, node.ID);
                            node.Product = ZWManager.Instance.GetNodeProductName(homeID, node.ID);
                            node.Location = ZWManager.Instance.GetNodeLocation(homeID, node.ID);
                            node.Name = ZWManager.Instance.GetNodeName(homeID, node.ID);
                        }
                        break;
                    }

                case NotificationType.NodeEvent:
                    {
                        break;
                    }

                case NotificationType.PollingDisabled:
                    {
                        Debug.WriteLine("Polling disabled notification");
                        break;
                    }

                case NotificationType.PollingEnabled:
                    {
                        Debug.WriteLine("Polling enabled notification");
                        break;
                    }

                case NotificationType.DriverReady:
                    {
                        CurrentStatus = $"Initializing...driver with Home ID 0x{notification.HomeId.ToString("X8")} is ready.";
                        break;
                    }
                case NotificationType.DriverFailed:
                    {
                        Debug.WriteLine("Driver failed for HomeID " + homeID.ToString());
                        break;
                    }
                case NotificationType.DriverRemoved:
                    {
                        var nodes = GetNodes(homeID).ToArray();
                        foreach (var node in nodes)
                            m_nodeList.Remove(node);
                        break;
                    }
                case NotificationType.NodeQueriesComplete:
                    {
                        // as an example, enable query of BASIC info (CommandClass = 0x20)
                        Node node = GetNode(homeID, nodeId);
                        //if (node != null)
                        //{
                        //    foreach (ZWValueID vid in node.Values)
                        //    {
                        //        if (vid.GetCommandClassId() == 0x84)	// remove this "if" to poll all values
                        //            m_manager.EnablePoll(vid);
                        //    }
                        //}
                        CurrentStatus = $"Initializing...node {node.ID} query complete.";
                        break;
                    }
                case NotificationType.EssentialNodeQueriesComplete:
                    {
                        Node node = GetNode(homeID, nodeId);
                        CurrentStatus = $"Initializing...node {node.ID} essential queries complete.";
                        break;
                    }
                case NotificationType.AllNodesQueried:
                    {
                        CurrentStatus = "Ready:  All nodes queried.";
                        ZWManager.Instance.WriteConfig(homeID);
                        break;
                    }
                case NotificationType.AllNodesQueriedSomeDead:
                    {
                        CurrentStatus = "Ready:  All nodes queried but some are dead.";
                        ZWManager.Instance.WriteConfig(homeID);
                        break;
                    }
                case NotificationType.AwakeNodesQueried:
                    {
                        CurrentStatus = "Ready:  Awake nodes queried (but not some sleeping nodes).";
                        ZWManager.Instance.WriteConfig(homeID);
                        break;
                    }
                default:
                    {
                        Debug.WriteLine($"******Notification '{type}' not Handled @ ID: {nodeId}");
                        break;
                    }
            }

            //NodeGridView.Refresh();
            //NodeGridView.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None;
            //NodeGridView.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.DisplayedCells;
        }
        private string m_CurrentStatus;

        public string CurrentStatus
        {
            get { return m_CurrentStatus; }
            private set { m_CurrentStatus = value; OnPropertyChanged(); }
        }

        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }


        /// <summary>
        /// Gets the value.
        /// </summary>
        /// <param name="v">The v.</param>
        /// <returns></returns>
        private string GetValue(ZWValueID v)
        {
            switch (v.Type)
            {
                case ZWValueType.Bool:
                    bool r1;
                    ZWManager.Instance.GetValueAsBool(v, out r1);
                    return r1.ToString();
                case ZWValueType.Byte:
                    byte r2;
                    ZWManager.Instance.GetValueAsByte(v, out r2);
                    return r2.ToString();
                case ZWValueType.Decimal:
                    decimal r3;
                    string r3s;
                    ZWManager.Instance.GetValueAsString(v, out r3s);
                    return r3s;
                //throw new NotImplementedException("Decimal");
                //m_manager.GetValueAsDecimal(v, out r3);
                //return r3.ToString();
                case ZWValueType.Int:
                    Int32 r4;
                    ZWManager.Instance.GetValueAsInt(v, out r4);
                    return r4.ToString();
                case ZWValueType.List:
                    string[] r5;
                    ZWManager.Instance.GetValueListItems(v, out r5);
                    string r6 = "";
                    foreach (string s in r5)
                    {
                        r6 += s;
                        r6 += "/";
                    }
                    return r6;
                case ZWValueType.Schedule:
                    return "Schedule";
                case ZWValueType.Short:
                    short r7;
                    ZWManager.Instance.GetValueAsShort(v, out r7);
                    return r7.ToString();
                case ZWValueType.String:
                    string r8;
                    ZWManager.Instance.GetValueAsString(v, out r8);
                    return r8;
                default:
                    return "";
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


        private bool m_securityEnabled = false;

        public bool SecurityEnabled
        {
            get { return m_securityEnabled; }
        }

        private ObservableCollection<Node> m_nodeList = new ObservableCollection<Node>();

        public event PropertyChangedEventHandler PropertyChanged;

        public IEnumerable<Node> Nodes => m_nodeList;
    }
}
