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

        public static readonly DependencyProperty PartiallyConnectedProperty = DependencyProperty.Register("PartiallyConnected", typeof(bool), typeof(ConnectionStateLight), null);
        public static readonly DependencyProperty ConnectedProperty = DependencyProperty.Register("Connected", typeof(bool), typeof(ConnectionStateLight), null);
        public static readonly DependencyProperty NotConnectedProperty = DependencyProperty.Register("NotConnected", typeof(bool), typeof(ConnectionStateLight), null);

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
                SetValue(ConnectedProperty, _connectionState == null ? false : _connectionState.SilentMonitorConnected && _connectionState.CANBusConnected);
                SetValue(NotConnectedProperty, _connectionState == null ? true : !_connectionState.SilentMonitorConnected && !_connectionState.CANBusConnected);
                SetValue(PartiallyConnectedProperty, _connectionState == null ? false : (!Connected && !NotConnected && _connectionState.CANBusConnected));
               
                _connectionState.PropertyChanged += (s, e) =>
                {
                    SetValue(ConnectedProperty, _connectionState == null ? false : _connectionState.SilentMonitorConnected && _connectionState.CANBusConnected);
                    SetValue(NotConnectedProperty, _connectionState == null ? true : !_connectionState.SilentMonitorConnected && !_connectionState.CANBusConnected);
                    SetValue(PartiallyConnectedProperty, _connectionState == null ? false :
                        (!Connected && !NotConnected && _connectionState.CANBusConnected));


                };
            }
        }
        public bool PartiallyConnected
        {
            get { return (bool) GetValue(PartiallyConnectedProperty); }
        }

        public bool Connected
        {
            get { return (bool)GetValue(ConnectedProperty); }
        }

        public bool NotConnected
        {
            get { return (bool)GetValue(NotConnectedProperty); }
        }
    }
}
