#include "SilentMonitorCommunicator.h"
#include "nlohmann/json.hpp"
#include "GetStateCommand.h"
#include "Logger.h"
using json = nlohmann::json;

SilentMonitorCommunicator::SilentMonitorCommunicator(std::shared_ptr<BluetoothServer> bluetoothServer) : _bluetoothServer(std::move(bluetoothServer))
{
}

void SilentMonitorCommunicator::Run() {
	while (!_stopped) {
		std::string request = _bluetoothServer->_receivedCommands.getNext();
		Logger::Info("request got: " + request);
		auto reqjson = json::parse(request);
		if (reqjson["CommandType"].get<std::string>() == "GetState") {
			GetStateCommand cmd;
			_bluetoothServer->SendCommand(cmd.GetResponse());
		}
	}
}