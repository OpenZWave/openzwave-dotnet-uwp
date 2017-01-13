using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using OpenZWave;
using System.Collections.ObjectModel;
using System.Diagnostics;
using Windows.Devices.Enumeration;

namespace OZWAppx
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private static ZWOptions m_options = null;

        public static ZWOptions Options
        {
            get { return m_options; }
        }

        private static ZWManager m_manager = null;

        public static ZWManager Manager
        {
            get { return m_manager; }
        }

        private bool m_securityEnabled = false;

        public bool SecurityEnabled
        {
            get { return m_securityEnabled; }
        }

        private UInt32 m_homeId = 0;
        private ObservableCollection<Node> m_nodeList = new ObservableCollection<Node>();
        private Byte m_rightClickNode = 0xff;

        public IEnumerable<Node> Nodes => m_nodeList;
        
        public MainPage()
        {
            this.InitializeComponent();

            ZWManager manager = new ZWManager();
            NodeGridView.ItemsSource = m_nodeList;
            m_options = new ZWOptions();
            var userPath = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
            m_options.Create(@"config\", userPath, @"");

            // Add any app specific options here...
            m_options.AddOptionInt("SaveLogLevel", (int)ZWLogLevel.Detail);
            // ordinarily, just write "Detail" level messages to the log
            m_options.AddOptionInt("QueueLogLevel", (int)ZWLogLevel.Debug);
            // save recent messages with "Debug" level messages to be dumped if an error occurs
            m_options.AddOptionInt("DumpTriggerLevel", (int)ZWLogLevel.Error);
            // only "dump" Debug  to the log emessages when an error-level message is logged

            // Lock the options
            m_options.Lock();

            // Create the OpenZWave Manager
            m_manager = new ZWManager();
            m_manager.Create();
            m_manager.OnNotification += OnNodeNotification;

            // Add a driver
            //m_driverPort = @"\\.\COM5";
            //m_manager.AddDriver(m_driverPort);
            GetSerialPorts();
        }
        private async void GetSerialPorts()
        {
            var serialPortSelector = Windows.Devices.SerialCommunication.SerialDevice.GetDeviceSelector();
            var devices = await DeviceInformation.FindAllAsync(serialPortSelector);
            List<string> serialPorts = new List<string>();
            foreach(var item in devices)
            {
                serialPorts.Add(item.Id);
            }
            SerialPortList.ItemsSource = serialPorts;
            SerialPortList.SelectionChanged += SerialPortList_SelectionChanged;
            if (!serialPorts.Any())
            {
                new Windows.UI.Popups.MessageDialog("No serial ports found").ShowAsync();
            }
            else if (serialPorts.Count == 1)
                SerialPortList.SelectedIndex = 0;
        }

        private void SerialPortList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var id = e.AddedItems?.FirstOrDefault() as string;
            if(id != null)
            {
                m_manager.AddDriver(id);
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
            var _ = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                NotificationHandler(notification);
            });
        }

        /// <summary>
        /// The notification handler.
        /// </summary>
        private void NotificationHandler(ZWNotification notification)
        {
            switch (notification.GetType())
            {
                case NotificationType.ValueAdded:
                    {
                        Node node = GetNode(notification.GetHomeId(), notification.GetNodeId());
                        if (node != null)
                        {
                            node.AddValue(notification.GetValueID());
                        }
                        break;
                    }

                case NotificationType.ValueRemoved:
                    {
                        Node node = GetNode(notification.GetHomeId(), notification.GetNodeId());
                        if (node != null)
                        {
                            node.RemoveValue(notification.GetValueID());
                        }
                        break;
                    }

                case NotificationType.ValueChanged:
                    {
                        Debug.WriteLine("Value Changed");
                        ZWValueID v = notification.GetValueID();
                        Debug.WriteLine("  Node : " + v.GetNodeId().ToString());
                        Debug.WriteLine("  CC   : " + v.GetCommandClassId().ToString());
                        Debug.WriteLine("  Type : " + v.GetType().ToString());
                        Debug.WriteLine("  Index: " + v.GetIndex().ToString());
                        Debug.WriteLine("  Inst : " + v.GetInstance().ToString());
                        Debug.WriteLine("  Value: " + GetValue(v).ToString());
                        Debug.WriteLine("  Label: " + m_manager.GetValueLabel(v));
                        Debug.WriteLine("  Help : " + m_manager.GetValueHelp(v));
                        Debug.WriteLine("  Units: " + m_manager.GetValueUnits(v));
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
                        if (GetNode(notification.GetHomeId(), notification.GetNodeId()) == null)
                        {
                            Node node = new Node();
                            node.ID = notification.GetNodeId();
                            node.HomeID = notification.GetHomeId();
                            m_nodeList.Add(node);
                        }
                        break;
                    }

                case NotificationType.NodeNew:
                    {
                        // Add the new node to our list (and flag as uninitialized)
                        Node node = new Node();
                        node.ID = notification.GetNodeId();
                        node.HomeID = notification.GetHomeId();
                        m_nodeList.Add(node);
                        break;
                    }

                case NotificationType.NodeRemoved:
                    {
                        foreach (Node node in m_nodeList)
                        {
                            if (node.ID == notification.GetNodeId())
                            {
                                m_nodeList.Remove(node);
                                break;
                            }
                        }
                        break;
                    }

                case NotificationType.NodeProtocolInfo:
                    {
                        Node node = GetNode(notification.GetHomeId(), notification.GetNodeId());
                        if (node != null)
                        {
                            node.Label = m_manager.GetNodeType(m_homeId, node.ID);
                        }
                        break;
                    }

                case NotificationType.NodeNaming:
                    {
                        Node node = GetNode(notification.GetHomeId(), notification.GetNodeId());
                        if (node != null)
                        {
                            node.Manufacturer = m_manager.GetNodeManufacturerName(m_homeId, node.ID);
                            node.Product = m_manager.GetNodeProductName(m_homeId, node.ID);
                            node.Location = m_manager.GetNodeLocation(m_homeId, node.ID);
                            node.Name = m_manager.GetNodeName(m_homeId, node.ID);
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
                        m_homeId = notification.GetHomeId();
                        toolStripStatusLabel1.Text = "Initializing...driver with Home ID 0x" + m_homeId.ToString("X8") +
                                                     " is ready.";
                        break;
                    }
                case NotificationType.NodeQueriesComplete:
                    {
                        // as an example, enable query of BASIC info (CommandClass = 0x20)
                        Node node = GetNode(notification.GetHomeId(), notification.GetNodeId());
                        //if (node != null)
                        //{
                        //    foreach (ZWValueID vid in node.Values)
                        //    {
                        //        if (vid.GetCommandClassId() == 0x84)	// remove this "if" to poll all values
                        //            m_manager.EnablePoll(vid);
                        //    }
                        //}
                        toolStripStatusLabel1.Text = "Initializing...node " + node.ID + " query complete.";
                        break;
                    }
                case NotificationType.EssentialNodeQueriesComplete:
                    {
                        Node node = GetNode(notification.GetHomeId(), notification.GetNodeId());
                        toolStripStatusLabel1.Text = "Initializing...node " + node.ID + " essential queries complete.";
                        break;
                    }
                case NotificationType.AllNodesQueried:
                    {
                        toolStripStatusLabel1.Text = "Ready:  All nodes queried.";
                        m_manager.WriteConfig(notification.GetHomeId());
                        break;
                    }
                case NotificationType.AllNodesQueriedSomeDead:
                    {
                        toolStripStatusLabel1.Text = "Ready:  All nodes queried but some are dead.";
                        m_manager.WriteConfig(notification.GetHomeId());
                        break;
                    }
                case NotificationType.AwakeNodesQueried:
                    {
                        toolStripStatusLabel1.Text = "Ready:  Awake nodes queried (but not some sleeping nodes).";
                        m_manager.WriteConfig(notification.GetHomeId());
                        break;
                    }
                default:
                    {
                        Debug.WriteLine($"******Notification '{notification.GetType()}' not Handled @ ID: {notification.GetNodeId()}");
                        break;
                    }
            }

            //NodeGridView.Refresh();
            //NodeGridView.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None;
            //NodeGridView.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.DisplayedCells;
        }
        /// <summary>
        /// Gets the value.
        /// </summary>
        /// <param name="v">The v.</param>
        /// <returns></returns>
        private string GetValue(ZWValueID v)
        {
            switch (v.GetType())
            {
                case ZWValueType.Bool:
                    bool r1;
                    m_manager.GetValueAsBool(v, out r1);
                    return r1.ToString();
                case ZWValueType.Byte:
                    byte r2;
                    m_manager.GetValueAsByte(v, out r2);
                    return r2.ToString();
                case ZWValueType.Decimal:
                    decimal r3;
                    string r3s;
                    m_manager.GetValueAsString(v, out r3s);
                    return r3s;
                    //throw new NotImplementedException("Decimal");
                    //m_manager.GetValueAsDecimal(v, out r3);
                    //return r3.ToString();
                case ZWValueType.Int:
                    Int32 r4;
                    m_manager.GetValueAsInt(v, out r4);
                    return r4.ToString();
                case ZWValueType.List:
                    string[] r5;
                    m_manager.GetValueListItems(v, out r5);
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
                    m_manager.GetValueAsShort(v, out r7);
                    return r7.ToString();
                case ZWValueType.String:
                    string r8;
                    m_manager.GetValueAsString(v, out r8);
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

        private void SaveConfiguration_Click(object sender, RoutedEventArgs e)
        {
            m_manager.WriteConfig(m_homeId);
        }

        private void PowerOn_ContextMenuClick(object sender, RoutedEventArgs e)
        {
            var node = (sender as FrameworkElement).DataContext as Node;
            //var value = new ZWValueID(node.HomeID, node.ID, ZWValueGenre.Basic, 0x20,  )
            //m_manager.SetValue(value, 0x255);
        }

        private void PowerOff_ContextMenuClick(object sender, RoutedEventArgs e)
        {
            //m_manager.SetNodeOff(m_homeId, m_rightClickNode);
        }
    }
}
