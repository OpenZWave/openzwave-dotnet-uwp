using OpenZWave;
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
    public sealed partial class ControllerView : Page
    {
        public ControllerView()
        {
            this.InitializeComponent();
        }

        private void SaveConfiguration_Click(object sender, RoutedEventArgs e)
        {
            var homeIds = MainViewModel.Instance.Nodes.Select(n => n.HomeID).Distinct();
            foreach(var homeId in homeIds)
                ZWManager.Instance.WriteConfig(homeId);
        }

        public MainViewModel VM => MainViewModel.Instance;
    }
}
