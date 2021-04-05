using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using UWPApp.ViewModels;
// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace UWPApp
{
    public sealed partial class ConnectionStateLight : UserControl, INotifyPropertyChanged
    {
        private ConnectionStateViewModel _connectionState;

        public event PropertyChangedEventHandler PropertyChanged;

        public ConnectionStateLight()
        {
            this.InitializeComponent();
        }

        public ConnectionStateViewModel ConnectionState
        {
            get { return _connectionState; }
            set
            {
                _connectionState = value;
                _connectionState.PropertyChanged += (s, e) =>
                {
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("PartiallyConnected"));
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Connected"));
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("NotConnected"));

                };
            }
        }
        public bool PartiallyConnected
        {
            get { return _connectionState == null ? false : _connectionState.SilentMonitorConnected; }
        }

        public bool Connected
        {
            get { return _connectionState == null ? false : _connectionState.SilentMonitorConnected && _connectionState.CANBusConnected; }
        }

        public bool NotConnected
        {
            get { return _connectionState == null ? true : !_connectionState.SilentMonitorConnected && !_connectionState.CANBusConnected; }
        }
    }
}
