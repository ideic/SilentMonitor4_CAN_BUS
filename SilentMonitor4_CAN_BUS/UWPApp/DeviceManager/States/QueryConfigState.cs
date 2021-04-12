using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UWPApp.ViewModels;
using Windows.Data.Json;

namespace UWPApp.DeviceManager.States
{
    public class QueryConfigState : ICommunicationState
    {
        private SilentMonitorCommunicator _communicator;
        private ConnectionStateViewModel _connectionState;

        public QueryConfigState(SilentMonitorCommunicator communicator, ConnectionStateViewModel connectionState)
        {
            _communicator = communicator;
            _connectionState = connectionState;
        }
        public void Execute()
        {
            JsonObject jsonObject = new JsonObject();
            jsonObject["CommandType"] = JsonValue.CreateStringValue("GetConfig");

            try
            {
                var response = _communicator.Send(jsonObject.Stringify());

                var jsonResult = JsonObject.Parse(response);
                if (jsonResult["Status"].GetString() == "OK")
                {
                    _connectionState.WifiHost = jsonResult["WifiHost"].GetString();
                    _connectionState.WifiPort = jsonResult["WifiPort"].GetString();
                }
                else
                {
                    _connectionState.LastError = jsonResult["ErrorMessage"].GetString();

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
