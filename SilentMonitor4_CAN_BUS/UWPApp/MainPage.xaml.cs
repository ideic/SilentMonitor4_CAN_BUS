using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using UWPApp.DeviceManager;
using UWPApp.ViewModels;
using Windows.ApplicationModel.Background;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.Rfcomm;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace UWPApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private RfcommConnectionTrigger trigger;
        private BackgroundTaskRegistration _taskRegistration;
        private DeviceWatcher _deviceWatcher;
        private SilentMonitorCommunicator _silentMonitorCommunnicator;

        public MainPage()
        {
            this.InitializeComponent();
            ConnectionState = new ConnectionStateViewModel(this.Dispatcher);
            
            trigger = new RfcommConnectionTrigger();
            //trigger.OutboundConnection
            //trigger.RemoteHostName
            this.Loaded += MainPage_Loaded;

           
        }

        private async void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            _silentMonitorCommunnicator = new SilentMonitorCommunicator(ConnectionState);
            _silentMonitorCommunnicator.InitBluetoothDeviceConnection();
            // Applications registering for background trigger must request for permission.
            BackgroundAccessStatus backgroundAccessStatus = await BackgroundExecutionManager.RequestAccessAsync();

            var builder = new BackgroundTaskBuilder();
            //builder.TaskEntryPoint = "UWPApp.DeviceManager.RfcommClientTask";
            builder.SetTrigger(trigger);
            builder.Name = "RFCommClientTask";
        }

        private void Setting_Click(object sender, RoutedEventArgs e)
        {

        }

        public ConnectionStateViewModel ConnectionState { get; set; }

        private void CANWifi_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            var typicalSSIDs = new List<string> { "CLKDevices" };
            var suitableItems = new List<string>();
            
            foreach(var ssid in typicalSSIDs)
            {
                if (ssid.Contains(sender.Text, StringComparison.OrdinalIgnoreCase))
                {
                    suitableItems.Add(ssid);
                }
            }
            if (suitableItems.Count == 0)
            {
                suitableItems.Add("No results found");
            }
            sender.ItemsSource = suitableItems;

        }

        private void CANWifi_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            sender.Text = args.SelectedItem.ToString();
            //SuggestionOutput.Text = args.SelectedItem.ToString();
        }

       

        private void CANWifiPort_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            var typicalSSIDs = new List<string> { "35000" };
            var suitableItems = new List<string>();

            foreach (var ssid in typicalSSIDs)
            {
                if (ssid.Contains(sender.Text, StringComparison.OrdinalIgnoreCase))
                {
                    suitableItems.Add(ssid);
                }
            }
            if (suitableItems.Count == 0)
            {
                suitableItems.Add("No results found");
            }
            sender.ItemsSource = suitableItems;

        }

        private void CANWifiPort_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            sender.Text = args.SelectedItem.ToString();
            //SuggestionOutput.Text = args.SelectedItem.ToString();
        }
        private void Flyout_Opened(object sender, object e)
        {
            CANWifi.Focus(FocusState.Programmatic);
            
        }


        private void ZoomSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            try
            {
                ContentArea.ChangeView(null, null, (float)e.NewValue / 100);
            }
            catch (LayoutCycleException)
            {
                ContentArea.ChangeView(null, null, (float)e.OldValue / 100);
            }
            
        }
    }
}
