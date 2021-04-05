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

        public MainPage()
        {
            this.InitializeComponent();
            ConnectionState = ConnectionStateViewModel.Instance;
            
            trigger = new RfcommConnectionTrigger();
            //trigger.OutboundConnection
            //trigger.RemoteHostName
            this.Loaded += MainPage_Loaded;

           
        }

        private async void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            StartBluetoothDeviceWatcher();
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

        private void StartBluetoothDeviceWatcher()
        {
            ConnectionState.AvailableBluetoothDeviceCollection.Clear();
            // Request additional properties
            string[] requestedProperties = new string[] { "System.Devices.Aep.DeviceAddress", "System.Devices.Aep.IsConnected" };

            _deviceWatcher = DeviceInformation.CreateWatcher("(System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\")",
                                                            requestedProperties,
                                                            DeviceInformationKind.AssociationEndpoint);

            // Hook up handlers for the watcher events before starting the watcher
            _deviceWatcher.Added += new TypedEventHandler<DeviceWatcher, DeviceInformation>(async (watcher, deviceInfo) =>
            {
                // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    // Make sure device name isn't blank
                    if (deviceInfo.Name != "")
                    {
                        ConnectionState.AvailableBluetoothDeviceCollection.Add(new BluetoothDeviceDisplay(deviceInfo));
                    }
                });
            });

            _deviceWatcher.Updated += new TypedEventHandler<DeviceWatcher, DeviceInformationUpdate>(async (watcher, deviceInfoUpdate) =>
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    foreach (var rfcommInfoDisp in ConnectionState.AvailableBluetoothDeviceCollection)
                    {
                        if (rfcommInfoDisp.Id == deviceInfoUpdate.Id)
                        {
                            rfcommInfoDisp.Update(deviceInfoUpdate);
                            break;
                        }
                    }
                    bluetoothDevicesListView.SelectedItem = this.ConnectionState.SelectedBluetoothDevice;
                });
            });

            _deviceWatcher.EnumerationCompleted += new TypedEventHandler<DeviceWatcher, Object>(async (watcher, obj) =>
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    bluetoothDevicesListView.SelectedItem = this.ConnectionState.SelectedBluetoothDevice;
                });

                
            });

            _deviceWatcher.Removed += new TypedEventHandler<DeviceWatcher, DeviceInformationUpdate>(async (watcher, deviceInfoUpdate) =>
            {
                // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
                await Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    // Find the corresponding DeviceInformation in the collection and remove it
                    foreach (var rfcommInfoDisp in ConnectionState.AvailableBluetoothDeviceCollection)
                    {
                        if (rfcommInfoDisp.Id == deviceInfoUpdate.Id)
                        {
                            ConnectionState.AvailableBluetoothDeviceCollection.Remove(rfcommInfoDisp);
                            break;
                        }
                    }

                   
                });
            });

            _deviceWatcher.Stopped += new TypedEventHandler<DeviceWatcher, Object>(async (watcher, obj) =>
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                {
                    ConnectionState.AvailableBluetoothDeviceCollection.Clear();
                });
            });

            _deviceWatcher.Start();
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

        private async void bluetoothDevicesListView_DoubleTapped(object sender, DoubleTappedRoutedEventArgs e)
        {
            bluetoothConnectionMessage.Visibility = Visibility.Collapsed;
            this.ConnectionState.SelectedBluetoothDevice = (BluetoothDeviceDisplay)bluetoothDevicesListView.SelectedItem;

            var deviceInfoDisp = this.ConnectionState.SelectedBluetoothDevice;

            DeviceAccessStatus accessStatus = DeviceAccessInformation.CreateFromId(deviceInfoDisp.Id).CurrentStatus;
            if (accessStatus == DeviceAccessStatus.DeniedByUser)
            {
                return;
            }
            BluetoothDevice bluetoothDevice;
            // If not, try to get the Bluetooth device
            try
            {
                bluetoothDevice = await BluetoothDevice.FromIdAsync(deviceInfoDisp.Id);
            }
            catch (Exception ex)
            {
                bluetoothConnectionMessage.Visibility = Visibility.Visible;
                bluetoothConnectionMessage.Text = "Error: " + ex.Message;
                return;
            }
           if (bluetoothDevice == null)
            {
                bluetoothConnectionMessage.Visibility = Visibility.Visible;
                bluetoothConnectionMessage.Text = "Invalid Device";

                return;
            }

            // This should return a list of uncached Bluetooth services (so if the server was not active when paired, it will still be detected by this call
            var rfcommServices = await bluetoothDevice.GetRfcommServicesForIdAsync(RfcommServiceId.FromUuid(Constants.RfcommChatServiceUuid), BluetoothCacheMode.Uncached);

            RfcommDeviceService canBusService = null ;
            if (rfcommServices.Services.Count > 0)
            {
                canBusService = rfcommServices.Services[0];
            }

            if (canBusService == null)
            {
                bluetoothConnectionMessage.Visibility = Visibility.Visible;
                bluetoothConnectionMessage.Text = "canBusService is null";

                return;
            }

            // Do various checks of the SDP record to make sure you are talking to a device that actually supports the Bluetooth Rfcomm Chat Service
            var attributes = await canBusService.GetSdpRawAttributesAsync();
            if (!attributes.ContainsKey(Constants.SdpServiceNameAttributeId))
            {
                bluetoothConnectionMessage.Visibility = Visibility.Visible;
                bluetoothConnectionMessage.Text = "The Chat service is not advertising the Service Name attribute (attribute id=0x100). " +
                    "Please verify that you are running the CANBUS Sniffer device server.";
                return;
            }
            var attributeReader = DataReader.FromBuffer(attributes[Constants.SdpServiceNameAttributeId]);
            var attributeType = attributeReader.ReadByte();
            if (attributeType != Constants.SdpServiceNameAttributeType)
            {
                bluetoothConnectionMessage.Visibility = Visibility.Visible;
                bluetoothConnectionMessage.Text =
                    "The CANBUS Sniffer is using an unexpected format for the Service Name attribute. " +
                    "Please verify that you are running the BluetoothRfcommChat server.";
                return;
            }
            var serviceNameLength = attributeReader.ReadByte();

            // The Service Name attribute requires UTF-8 encoding.
            attributeReader.UnicodeEncoding = UnicodeEncoding.Utf8;

            //lock (this)
            //{
            //    chatSocket = new StreamSocket();
            //}
            //try
            //{
            //    await chatSocket.ConnectAsync(chatService.ConnectionHostName, chatService.ConnectionServiceName);

            //    SetChatUI(attributeReader.ReadString(serviceNameLength), bluetoothDevice.Name);
            //    chatWriter = new DataWriter(chatSocket.OutputStream);

            //    DataReader chatReader = new DataReader(chatSocket.InputStream);
            //    ReceiveStringLoop(chatReader);
            //}
            //catch (Exception ex) when ((uint)ex.HResult == 0x80070490) // ERROR_ELEMENT_NOT_FOUND
            //{
            //    rootPage.NotifyUser("Please verify that you are running the BluetoothRfcommChat server.", NotifyType.ErrorMessage);
            //    ResetMainUI();
            //}
            //catch (Exception ex) when ((uint)ex.HResult == 0x80072740) // WSAEADDRINUSE
            //{
            //    rootPage.NotifyUser("Please verify that there is no other RFCOMM connection to the same device.", NotifyType.ErrorMessage);
            //    ResetMainUI();
            //}
        }
    }
}
