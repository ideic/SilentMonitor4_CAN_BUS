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

        public static readonly DependencyProperty PartiallyConnectedProperty = DependencyProperty.RegisterAttached("PartiallyConnected", typeof(Visibility), typeof(ConnectionStateLight), new PropertyMetadata(Visibility.Collapsed));
        public static readonly DependencyProperty ConnectedProperty = DependencyProperty.Register("Connected", typeof(Visibility), typeof(ConnectionStateLight), new PropertyMetadata(Visibility.Collapsed));
        public static readonly DependencyProperty NotConnectedProperty = DependencyProperty.Register("NotConnected", typeof(Visibility), typeof(ConnectionStateLight), new PropertyMetadata(Visibility.Visible));
 
        public ConnectionStateLight()
        {
            this.InitializeComponent();
            SetProperties();
        }
        private void SetProperties()
        {
            if (_connectionState == null)
            {
                Connected = Visibility.Collapsed;
                NotConnected = Visibility.Visible;
                PartiallyConnected = Visibility.Collapsed;
            }
            else
            {
                Connected = (_connectionState.SilentMonitorConnected && _connectionState.CANBusConnected) ? Visibility.Visible : Visibility.Collapsed;
                NotConnected = (!_connectionState.SilentMonitorConnected && !_connectionState.CANBusConnected)? Visibility.Visible : Visibility.Collapsed;
                PartiallyConnected = ((Connected == Visibility.Collapsed) && (NotConnected==Visibility.Collapsed) && _connectionState.CANBusConnected) ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public ConnectionStateViewModel ConnectionState
        {
            get { return _connectionState; }
            set
            {
                _connectionState = value;
                SetProperties();
                _connectionState.PropertyChanged += (s, e) =>
                {
                    switch (e.PropertyName)
                    {
                        case "CANBusConnected":
                        case "SilentMonitorConnected":
                            {
                                SetProperties();
                            }
                            break;
                    }
                };
            }
        }
        public Visibility PartiallyConnected
        {
            get { return (Visibility) GetValue(PartiallyConnectedProperty); }
            set { SetValue(PartiallyConnectedProperty, value); }
        }

        public Visibility Connected
        {
            get { return (Visibility)GetValue(ConnectedProperty); }
            set { SetValue(ConnectedProperty, value); }
        }

        public Visibility NotConnected
        {
            get { return (Visibility)GetValue(NotConnectedProperty); }
            set { SetValue(NotConnectedProperty, value); }
        }
    }
}
