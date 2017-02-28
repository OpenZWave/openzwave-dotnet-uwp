using OpenZWave;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Popups;
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
    public sealed partial class ControllerView : Page
    {
        public ControllerView()
        {
            this.InitializeComponent();
        }

        private void SaveConfiguration_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach(var homeId in homeIds)
                ZWManager.Instance.WriteConfig(homeId);
        }

        public NodeWatcher VM => NodeWatcher.Instance;

        private void ResetSoft_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach (var homeId in homeIds)
                ZWManager.Instance.SoftReset(homeId);
        }

        private void ResetHard_Click(object sender, RoutedEventArgs e)
        {
            var dlg = new MessageDialog("Reset all connected controllers? This can't be undone", "Confirm");
            dlg.Commands.Add(new UICommand("OK", (s) =>
            {
                var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
                foreach (var homeId in homeIds)
                    ZWManager.Instance.ResetController(homeId);
            }));
            dlg.Commands.Add(new UICommand("Cancel"));
            var _ = dlg.ShowAsync();
        }

        private void ReceiveConfig_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach (var homeId in homeIds)
                ZWManager.Instance.ReceiveConfiguration(homeId);
        }

        private void RemoveNode_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach (var homeId in homeIds)
                ZWManager.Instance.RemoveNode(homeId);

            var dlg = new MessageDialog("Please follow the manufacturer's instructions to remove the Z-Wave device from the controller", "Remove mode enabled");
            var _ = dlg.ShowAsync();
        }

        private void AddDevice_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach (var homeId in homeIds)
                ZWManager.Instance.AddNode(homeId, false);

            var dlg = new MessageDialog("Please follow the manufacturer's instructions to add the Z-Wave device from the controller", "Add mode enabled");
            var _ = dlg.ShowAsync();
        }

        private void AddSecureDevice_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach (var homeId in homeIds)
                ZWManager.Instance.AddNode(homeId, true);

            var dlg = new MessageDialog("Please follow the manufacturer's instructions to add the Z-Wave device from the controller", "Add mode enabled");
            var _ = dlg.ShowAsync();
        }

        private void NewPrimary_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach (var homeId in homeIds)
                ZWManager.Instance.CreateNewPrimary(homeId);
        }

        private void TransferPrimary_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = NodeWatcher.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach (var homeId in homeIds)
                ZWManager.Instance.TransferPrimaryRole(homeId);
        }
    }
}
