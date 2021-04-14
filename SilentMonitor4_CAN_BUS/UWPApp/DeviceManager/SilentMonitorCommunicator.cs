using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using UWPApp.DeviceManager.States;
using UWPApp.ViewModels;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.Rfcomm;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace UWPApp.DeviceManager
{
    public class SilentMonitorCommunicator
    {
        private static DataWriter _dataWriter;
        private static DataReader _dataReader;
        private ConnectionStateViewModel _connectionState;
        private IViewContext _viewContext;
        private DeviceWatcher _deviceWatcher;

        internal string Send(string message)
        {
            //Request
            _dataWriter.WriteString(message);
            _dataWriter.StoreAsync().AsTask().Wait();

            //Response
            const uint responseSize = 4;
            uint size = _dataReader.LoadAsync(responseSize).AsTask().Result;
            if (size < responseSize)
            {
                throw new Exception("Disconnected");
            }

            uint stringLength = UInt32.Parse(_dataReader.ReadString(responseSize));
            uint actualStringLength = _dataReader.LoadAsync(stringLength).AsTask().Result;
            if (actualStringLength != stringLength)
            {
                throw new Exception("Disconnected during read");
            }
            return _dataReader.ReadString(stringLength);
        }

        internal void NextState (ICommunicationState nextState)
        {
            _currentState = nextState;
            _viewContext.PostCommand(() => _currentState.Execute());
        }

        private Timer _timer;
        private ICommunicationState _currentState;

        public SilentMonitorCommunicator(ConnectionStateViewModel connectionState, IViewContext viewContext)
        {
            _connectionState = connectionState;
            _viewContext = viewContext;
        }

        internal void InitBluetoothDeviceConnection()
        {

            // Request additional properties
            string[] requestedProperties = new string[] { "System.Devices.Aep.DeviceAddress", "System.Devices.Aep.IsConnected" };

            //BL device, not BLE, https://docs.microsoft.com/en-us/windows/uwp/devices-sensors/aep-service-class-ids
            _deviceWatcher = DeviceInformation.CreateWatcher("(System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\")",
                                                            requestedProperties,
                                                            DeviceInformationKind.AssociationEndpoint);

            // Hook up handlers for the watcher events before starting the watcher
            _deviceWatcher.Added += new TypedEventHandler<DeviceWatcher, DeviceInformation>((watcher, deviceInfo) =>
            {
                // Make sure device name isn't blank
                if (deviceInfo.Name != "" && deviceInfo.Name.StartsWith("canbussniffer"))
                {
                    _deviceWatcher.Stop();
                    Connect2Bluetooth(deviceInfo);
                }

            });

            _deviceWatcher.Updated += new TypedEventHandler<DeviceWatcher, DeviceInformationUpdate>((watcher, deviceInfoUpdate) =>
            {

            });

            _deviceWatcher.Stopped += new TypedEventHandler<DeviceWatcher, Object>((watcher, obj) =>
            {

            });

            if (!_connectionState.CANBusConnected)
            {
                _deviceWatcher.Start();
            }

        }

        private async void Connect2Bluetooth(DeviceInformation deviceInfo)
        {
            DeviceAccessStatus accessStatus = DeviceAccessInformation.CreateFromId(deviceInfo.Id).CurrentStatus;
            if (accessStatus == DeviceAccessStatus.DeniedByUser)
            {
                return;
            }
            BluetoothDevice bluetoothDevice;
            // If not, try to get the Bluetooth device
            try
            {
                bluetoothDevice = await BluetoothDevice.FromIdAsync(deviceInfo.Id);
            }
            catch (Exception ex)
            {
                _connectionState.LastError = "Error: " + ex.Message;
                StartTimer((x) => InitBluetoothDeviceConnection(), TimeSpan.FromSeconds(30));
                return;
            }
            if (bluetoothDevice == null)
            {
                StartTimer((x) => InitBluetoothDeviceConnection(), TimeSpan.FromSeconds(30));
                _connectionState.LastError = "Invalid Device";
                return;
            }

            // This should return a list of uncached Bluetooth services (so if the server was not active when paired, it will still be detected by this call
            var rfcommServices = await bluetoothDevice.GetRfcommServicesAsync(BluetoothCacheMode.Uncached);

            RfcommDeviceService canBusService = null;
            if (rfcommServices.Services.Count > 0)
            {
                canBusService = rfcommServices.Services[0];
            }

            if (canBusService == null)
            {
                _connectionState.LastError = "canBusService is null";
                StartTimer((x) => InitBluetoothDeviceConnection(), TimeSpan.FromSeconds(30));
                return;
            }
            StreamSocket canBusSocket;
            lock (this)
            {
                canBusSocket = new StreamSocket();
            }
            try
            {
                await canBusSocket.ConnectAsync(canBusService.ConnectionHostName, canBusService.ConnectionServiceName);

                _dataWriter = new DataWriter(canBusSocket.OutputStream);

                _dataReader = new DataReader(canBusSocket.InputStream);
            }
            catch (Exception ex) when ((uint)ex.HResult == 0x80070490) // ERROR_ELEMENT_NOT_FOUND
            {
                _connectionState.LastError = "Please verify that you are running the BluetoothRfcommChat server.";
                StartTimer((x) => InitBluetoothDeviceConnection(), TimeSpan.FromSeconds(30));
                return;
            }
            catch (Exception ex) when ((uint)ex.HResult == 0x80072740) // WSAEADDRINUSE
            {
                _connectionState.LastError = "Please verify that there is no other RFCOMM connection to the same device.";
                StartTimer((x) => InitBluetoothDeviceConnection(), TimeSpan.FromSeconds(30));
                return;
            }
            catch (Exception ex)
            {
                _connectionState.LastError = "Error: " + ex.Message;
                StartTimer((x) => InitBluetoothDeviceConnection(), TimeSpan.FromSeconds(30));
                return;
            }
            _connectionState.CANBusConnected = true;
            NextState(new QueryDeviceState(this, _connectionState));
        }

        private void StartTimer(TimerCallback callback, TimeSpan duetime)
        {
           _timer = new Timer(callback, null, (int)duetime.TotalMilliseconds, Timeout.Infinite);
        }
    }
}
