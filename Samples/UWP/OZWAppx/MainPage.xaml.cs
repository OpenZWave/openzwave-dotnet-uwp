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
        
        public MainPage()
        {
            this.InitializeComponent();
            VM = new MainViewModel(this.Dispatcher);
            VM.Initialize();
            GetSerialPorts();
        }

        public MainViewModel VM { get; }

        private async void GetSerialPorts()
        {
            var serialPortSelector = Windows.Devices.SerialCommunication.SerialDevice.GetDeviceSelector();
            var devices = await DeviceInformation.FindAllAsync(serialPortSelector);
            var serialPorts = new List<DeviceInformation>();
            foreach(var item in devices)
            {
                serialPorts.Add(item);
            }
            SerialPortList.ItemsSource = serialPorts;
            SerialPortList.SelectionChanged += SerialPortList_SelectionChanged;
            if (!serialPorts.Any())
            {
                var _ = new Windows.UI.Popups.MessageDialog("No serial ports found").ShowAsync();
            }
            else if (serialPorts.Count == 1)
                SerialPortList.SelectedIndex = 0;
        }

        private void SerialPortList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var di = e.AddedItems?.FirstOrDefault() as DeviceInformation;
            if(di != null)
            {
                VM.AddDriver(di.Id);
            }
        }

        private void SaveConfiguration_Click(object sender, RoutedEventArgs e)
        {
            VM.SaveConfig();
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
