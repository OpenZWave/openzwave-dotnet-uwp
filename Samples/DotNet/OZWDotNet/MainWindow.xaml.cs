using OpenZWave.NetworkManager;
using OZWAppx;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OZWDotNet
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            VM = NodeWatcher.Instance ?? new NodeWatcher(this.Dispatcher);
            VM.Initialize();
            DataContext = this;
            ApplicationState.Instance.InitializeAsync().ContinueWith((t) =>
            {
                GetSerialPorts();
            }, System.Threading.Tasks.TaskScheduler.FromCurrentSynchronizationContext());
        }

        public NodeWatcher VM { get; }

        public ApplicationState AppState => ApplicationState.Instance;

        private void GetSerialPorts()
        {
            if (!ApplicationState.Instance.SerialPorts.Any())
            {
                MessageBox.Show("No serial ports found");
            }
            else if (ApplicationState.Instance.SerialPorts.Count == 1)
            {
                //hamburgerMenu.SelectedIndex = 0;
                ApplicationState.Instance.SerialPorts[0].IsActive = true; //Assume if there's only one port, that's the ZStick port
                //(hamburgerMenu.Content as Frame).Navigate(typeof(Views.DevicesView));
            }
            else
            {
                // hamburgerMenu.SelectedIndex = 1;
                // (hamburgerMenu.Content as Frame).Navigate(typeof(Views.SettingsView));
            }
        }

        private void NodeGridView_ItemClick(object sender, SelectionChangedEventArgs e)
        {

        }

        private void SaveConfiguration_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
