using OpenZWave;
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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace OZWAppx.Views
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class DeviceView : UserControl
    {
        public DeviceView()
        {
            this.InitializeComponent();
        }

        //protected override void OnNavigatedTo(NavigationEventArgs e)
        //{
        //    base.OnNavigatedTo(e);
        //    Node node = e.Parameter as Node;
        //    VM = new DeviceViewVM(node);
        //}


        public Node Node
        {
            get { return (Node)GetValue(NodeProperty); }
            set { SetValue(NodeProperty, value); }
        }

        public static readonly DependencyProperty NodeProperty =
            DependencyProperty.Register("Node", typeof(Node), typeof(DeviceView), new PropertyMetadata(null, OnNodePropertyChanged));

        private static void OnNodePropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            (d as DeviceView).VM.UpdateNode(e.NewValue as Node);
        }

        public DeviceViewVM VM { get; private set; } = new DeviceViewVM(null);


        private void PowerOn_ContextMenuClick(object sender, RoutedEventArgs e)
        {
            //var value = new ZWValueID(Node.HomeID, Node.Id, ZWValueGenre.Basic, 0x20,  )
            //m_manager.SetValue(value, 0x255);
        }

        private void PowerOff_ContextMenuClick(object sender, RoutedEventArgs e)
        {
            //m_manager.SetNodeOff(Node.HomeId, Node.Id);
        }

        private void RemoveButton_Click(object sender, RoutedEventArgs e)
        {
            var dlg = new MessageDialog("Remove device from controller?", "Confirm");
            dlg.Commands.Add(new UICommand("OK", (s) =>
            {
                VM.Remove();
            }));
            dlg.Commands.Add(new UICommand("Cancel"));
            var _ = dlg.ShowAsync();
        }
    }

    public class DeviceViewVM : INotifyPropertyChanged
    {
        public DeviceViewVM(Node node)
        {
            Node = node;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private static string[] GetDeviceClasses()
        {
            //TODO
            return null;
        }

        public void UpdateNode(Node node)
        {
            Node = node;
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Node)));
        }
        public Node Node { get; private set;}

        public void Remove()
        {
            if (Node != null && Node.HasNodeFailed)
            {
                ZWManager.Instance.RemoveFailedNode(Node.HomeID, Node.ID);
            }
        }
    }
    public class ZVValueIdConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            var v=  value as ZWValueID;
            if (v != null)
            {
                if (parameter as string == "Units")
                    return ZWManager.Instance.GetValueUnits(v);
                if (parameter as string == "Help")
                    return ZWManager.Instance.GetValueHelp(v);
                if (parameter as string == "Label")
                    return ZWManager.Instance.GetValueLabel(v);
                return GetValue(v);
            }
            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value.
        /// </summary>
        /// <param name="v">The v.</param>
        /// <returns></returns>
        private string GetValue(ZWValueID v)
        {
            var manager = ZWManager.Instance;
            switch (v.Type)
            {
                case ZWValueType.Bool:
                    bool r1;
                    manager.GetValueAsBool(v, out r1);
                    return r1.ToString();
                case ZWValueType.Byte:
                    byte r2;
                    manager.GetValueAsByte(v, out r2);
                    return r2.ToString();
                case ZWValueType.Decimal:
                    decimal r3;
                    string r3s;
                    manager.GetValueAsString(v, out r3s);
                    return r3s;
                //throw new NotImplementedException("Decimal");
                //m_manager.GetValueAsDecimal(v, out r3);
                //return r3.ToString();
                case ZWValueType.Int:
                    Int32 r4;
                    manager.GetValueAsInt(v, out r4);
                    return r4.ToString();
                case ZWValueType.List:
                    string value;
                    manager.GetValueListSelection(v, out value);
                    return value;
                    // string[] r5;
                    // m_manager.GetValueListItems(v, out r5);
                    // return string.Join("|", r5);
                case ZWValueType.Schedule:
                    return "Schedule";
                case ZWValueType.Short:
                    short r7;
                    manager.GetValueAsShort(v, out r7);
                    return r7.ToString();
                case ZWValueType.String:
                    string r8;
                    manager.GetValueAsString(v, out r8);
                    return r8;
                default:
                    return "";
            }
        }
    }
}
