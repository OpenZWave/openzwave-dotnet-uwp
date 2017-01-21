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
using Microsoft.Toolkit.Uwp.UI.Controls;

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
            VM = MainViewModel.Instance ?? new MainViewModel(this.Dispatcher);
            VM.Initialize().ContinueWith((t) =>
            {
                GetSerialPorts();
            }, System.Threading.Tasks.TaskScheduler.FromCurrentSynchronizationContext());
        }

        public MainViewModel VM { get; }

        private void GetSerialPorts()
        {
            if (!VM.SerialPorts.Any())
            {
                var _ = new Windows.UI.Popups.MessageDialog("No serial ports found").ShowAsync();
            }
            else if (VM.SerialPorts.Count == 1)
            {
                hamburgerMenu.SelectedIndex = 0;
                VM.SerialPorts[0].IsActive = true; //Assume if there's only one port, that's the ZStick port
                (hamburgerMenu.Content as Frame).Navigate(typeof(Views.DevicesView));
            }
            else
            {
                hamburgerMenu.SelectedIndex = 1;
                (hamburgerMenu.Content as Frame).Navigate(typeof(Views.SettingsView));
            }
        }

        private void HamburgerMenu_ItemClick(object sender, ItemClickEventArgs e)
        {
            var menu = sender as HamburgerMenu;
            var item = e.ClickedItem as HamburgerMenuItem;
            if (item.TargetPageType != null)
                (menu.Content as Frame).Navigate(item.TargetPageType);
        }
    }
}
