using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UWPApp.ViewModels;
using Windows.Data.Json;
namespace UWPApp.DeviceManager.States
{
    class QueryDeviceState : ICommunicationState
    {
        private SilentMonitorCommunicator _silentMonitorCommunicator;
        private ConnectionStateViewModel _connectionState;

        public QueryDeviceState(SilentMonitorCommunicator silentMonitorCommunicator, ConnectionStateViewModel connectionStateViewModel)
        {
            _silentMonitorCommunicator = silentMonitorCommunicator;
            _connectionState = connectionStateViewModel;
        }

        public void Execute()
        {
            JsonObject jsonObject = new JsonObject();
            jsonObject["CommandType"]= JsonValue.CreateStringValue("GetState");

            try
            {
                var response = _silentMonitorCommunicator.Send(jsonObject.Stringify()).Result;

                var jsonResult = JsonObject.Parse(response);
                if (jsonResult["Status"].GetString() == "OK")
                {
                    _connectionState.CANBusConnected = jsonResult["CANBus"].GetString() == "Connected";
                    _silentMonitorCommunicator.NextState(new QueryConfigState(_silentMonitorCommunicator, _connectionState));
                }
                else
                {
                    _connectionState.LastError = jsonResult["ErrorMessage"].GetString();
                }
            }
            catch (Exception ex)
            {
                _connectionState.LastError = ex.Message;
                _silentMonitorCommunicator.InitBluetoothDeviceConnection();
            }
        }
    }
}
