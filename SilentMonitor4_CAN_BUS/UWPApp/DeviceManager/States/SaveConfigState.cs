using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using UWPApp.ViewModels;
using Windows.Data.Json;

namespace UWPApp.DeviceManager.States
{
    public class SaveConfigState : ICommunicationState
    {
        private SilentMonitorCommunicator _communicator;
        private ConnectionStateViewModel _connectionState;

        public SaveConfigState(SilentMonitorCommunicator communicator, ConnectionStateViewModel connectionState)
        {
            _communicator = communicator;
            _connectionState = connectionState;
        }
        public void Execute()
        {
            JsonObject jsonObject = new JsonObject();
            jsonObject["CommandType"] = JsonValue.CreateStringValue("SaveConfig");
            jsonObject["WifiSSID"] = JsonValue.CreateStringValue(_connectionState.WifiSSID);
            jsonObject["WifiHost"] = JsonValue.CreateStringValue(_connectionState.WifiHost);
            jsonObject["WifiPort"] = JsonValue.CreateStringValue(_connectionState.WifiPort) ;
            try
            {
                var response = _communicator.Send(jsonObject.Stringify());

                var jsonResult = JsonObject.Parse(response);
                if (jsonResult["Status"].GetString() == "Restart")
                {
                    Thread.Sleep(TimeSpan.FromSeconds(5));
                    _communicator.InitBluetoothDeviceConnection();
                    return;
                }
                if (jsonResult["Status"].GetString() != "OK")
                {
                    _connectionState.LastError = jsonResult["ErrorMessage"].GetString();
                    return;
                }
            }
            catch (Exception ex)
            {
                _connectionState.LastError = ex.Message;
                _communicator.InitBluetoothDeviceConnection();
            }
        }
    }
}
