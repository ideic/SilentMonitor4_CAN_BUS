using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;

namespace UWPApp.ViewModels
{
    public class ConnectionStateViewModel : INotifyPropertyChanged
    {
        private static ConnectionStateViewModel _instance = new ConnectionStateViewModel();
        private bool _silentMonitorConnected = true;
        private bool _canBusConnected = false;
        private BluetoothDeviceDisplay _selectedBluetoothDevice;

        public event PropertyChangedEventHandler PropertyChanged;

        public static ConnectionStateViewModel Instance
        {
            get { return _instance; }
        }

        public bool SilentMonitorConnected
        {
            get { return _silentMonitorConnected; }
            set
            {
                _silentMonitorConnected = value;
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs("SilentMonitorConnected"));
            }
        }

        public bool CANBusConnected
        {
            get { return _canBusConnected; }
            set
            {
                _canBusConnected = value;
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs("CANBusConnected"));
            }
        }

        public ObservableCollection<BluetoothDeviceDisplay> AvailableBluetoothDeviceCollection
        {
            get;
            set;
        } = new ObservableCollection<BluetoothDeviceDisplay>();
        public BluetoothDeviceDisplay SelectedBluetoothDevice{
            get {
                if (_selectedBluetoothDevice == null)
                {
                    if (ApplicationData.Current.LocalSettings.Values["SelectedBluetoothDevice"] != null)
                    {
                        _selectedBluetoothDevice = AvailableBluetoothDeviceCollection.FirstOrDefault(d => d.Id == (string)ApplicationData.Current.LocalSettings.Values["SelectedBluetoothDevice"]);
                    }
                }
                return _selectedBluetoothDevice; 
            }
            set {
                if (_selectedBluetoothDevice == value) return;

                _selectedBluetoothDevice = value;
                ApplicationData.Current.LocalSettings.Values["SelectedBluetoothDevice"] = value.Id; ;
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs("SelectBluetoothDevice"));
            }
        }
    }
}
