using OpenZWave.NetworkManager;
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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace OZWAppx.Views
{
    public sealed partial class SettingsView : Page
    {
        public SettingsView()
        {
            this.InitializeComponent();
            if (!VM.SerialPorts.Any())
                NoDevicesPanel.Visibility = Visibility.Visible;
        }
        public ApplicationState VM => ApplicationState.Instance;

        private async void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                await VM.RefreshSerialPortsAsync();
            }
            catch { }
            NoDevicesPanel.Visibility = VM.SerialPorts.Any() ? Visibility.Collapsed : Visibility.Visible;
        }
    }
}
