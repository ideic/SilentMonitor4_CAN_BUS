using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI.Core;

namespace UWPApp.ViewModels
{
    public class ConnectionStateViewModel : INotifyPropertyChanged
    {
        private bool _silentMonitorConnected = false;
        private bool _canBusConnected = false;
        private string _lastError;
        private CoreDispatcher _dispatcher;
        private string _wifiHost;
        private string _wifiPort;

        public event PropertyChangedEventHandler PropertyChanged;

        public ConnectionStateViewModel(CoreDispatcher dispatcher)
        {
            _dispatcher = dispatcher;
        }
        public bool SilentMonitorConnected
        {
            get { return _silentMonitorConnected; }
            set
            {
                _silentMonitorConnected = value;
                if (!_dispatcher.HasThreadAccess)
                {
                    _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                        PropertyChanged.Invoke(this, new PropertyChangedEventArgs("SilentMonitorConnected"));
                     }).AsTask().Wait();
                }
                else
                {
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs("SilentMonitorConnected"));
                }
            }
        }

        public bool CANBusConnected
        {
            get { return _canBusConnected; }
            set
            {
                _canBusConnected = value;
                if (!_dispatcher.HasThreadAccess)
                {
                    _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        PropertyChanged.Invoke(this, new PropertyChangedEventArgs("CANBusConnected"));
                    }).AsTask().Wait();
                }
                else
                {
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs("CANBusConnected"));
                }
            }
        }

        public string LastError { 
            get {
                return _lastError;
            }
            internal set {
                _lastError = value;
                if (!_dispatcher.HasThreadAccess)
                {
                    _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        PropertyChanged.Invoke(this, new PropertyChangedEventArgs("LastError"));
                    }).AsTask().Wait();
                }
                else
                {
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs("LastError"));
                }
            }
        }

        public string WifiHost {
            get
            {
                return _wifiHost;
            }
            internal set
            {
                _wifiHost = value;
                if (!_dispatcher.HasThreadAccess)
                {
                    _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        PropertyChanged.Invoke(this, new PropertyChangedEventArgs("WifiHost"));
                    }).AsTask().Wait();
                }
                else
                {
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs("WifiHost"));
                }
            }
        }
        public string WifiPort
        {
            get
            {
                return _wifiPort;
            }
            internal set
            {
                _wifiPort = value;
                if (!_dispatcher.HasThreadAccess)
                {
                    _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        PropertyChanged.Invoke(this, new PropertyChangedEventArgs("WifiPort"));
                    }).AsTask().Wait();
                }
                else
                {
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs("WifiPort"));
                }
            }
        }
    }
}
