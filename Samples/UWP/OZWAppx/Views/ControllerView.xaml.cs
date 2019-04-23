using OpenZWave;
using OpenZWave.NetworkManager;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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

        public Controller Controller
        {
            get { return (Controller)GetValue(ControllerProperty); }
            set { SetValue(ControllerProperty, value); }
        }

        public static readonly DependencyProperty ControllerProperty =
            DependencyProperty.Register("Controller", typeof(Controller), typeof(ControllerView), new PropertyMetadata(null, OnControllerPropertyChanged));

        private static void OnControllerPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            (d as ControllerView).VM.UpdateController(e.NewValue as Controller);
        }

        public ControllerViewVM VM { get; private set; } = new ControllerViewVM();

        private void ResetSoft_Click(object sender, RoutedEventArgs e)
        {
            Controller.SoftReset();
        }

        private void ResetHard_Click(object sender, RoutedEventArgs e)
        {
            var dlg = new MessageDialog("Reset all connected controllers? This can't be undone", "Confirm");
            dlg.Commands.Add(new UICommand("OK", (s) =>
            {
                Controller.ResetController();
            }));
            dlg.Commands.Add(new UICommand("Cancel"));
            var _ = dlg.ShowAsync();
        }

        private void ReceiveConfig_Click(object sender, RoutedEventArgs e)
        {
            Controller.ReceiveConfiguration();
        }

        private void RemoveNode_Click(object sender, RoutedEventArgs e)
        {
            Controller.RemoveNode();
            var dlg = new MessageDialog("Please follow the manufacturer's instructions to remove the Z-Wave device from the controller", "Remove mode enabled");
            var _ = dlg.ShowAsync();
        }

        private void AddDevice_Click(object sender, RoutedEventArgs e)
        {
            Controller.AddNode(false);

            var dlg = new MessageDialog("Please follow the manufacturer's instructions to add the Z-Wave device from the controller", "Add mode enabled");
            var _ = dlg.ShowAsync();
        }

        private void AddSecureDevice_Click(object sender, RoutedEventArgs e)
        {
            Controller.AddNode(true);

            var dlg = new MessageDialog("Please follow the manufacturer's instructions to add the Z-Wave device from the controller", "Add mode enabled");
            var _ = dlg.ShowAsync();
        }

        private void NewPrimary_Click(object sender, RoutedEventArgs e)
        {
            Controller.CreateNewPrimary();
        }

        private void TransferPrimary_Click(object sender, RoutedEventArgs e)
        {
            Controller.TransferPrimaryRole();
        }
    }

    public class ControllerViewVM : INotifyPropertyChanged
    {
        public ControllerViewVM()
        {
        }

        public void UpdateController(Controller controller)
        {
            Controller = controller;
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Controller)));
        }
        public Controller Controller { get; private set; }

        public event PropertyChangedEventHandler PropertyChanged;
        
    }
}
