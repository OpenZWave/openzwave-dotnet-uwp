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
            VM = MainViewModel.Instance ?? new MainViewModel(this.Dispatcher);
            GetSerialPorts();
            DataContext = this;
        }

        public MainViewModel VM { get; }

        private void GetSerialPorts()
        {
            var serialPorts = System.IO.Ports.SerialPort.GetPortNames();
            SerialPortList.ItemsSource = serialPorts;
            SerialPortList.SelectionChanged += SerialPortList_SelectionChanged;
            if (!serialPorts.Any())
            {
                MessageBox.Show("No serial ports found");
            }
            else if (serialPorts.Length == 1)
                SerialPortList.SelectedIndex = 0;
        }

        private void SerialPortList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var portName = e.AddedItems?.OfType<string>().FirstOrDefault() as string;
            if (portName != null)
            {
                VM.AddDriver(portName);
            }
        }

        private void SaveConfiguration_Click(object sender, RoutedEventArgs e)
        {
            VM.SaveConfig();
        }

        private void NodeGridView_ItemClick(object sender, SelectionChangedEventArgs e)
        {

        }
    }
}
