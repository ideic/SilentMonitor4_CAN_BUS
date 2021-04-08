#include "SilentMonitorCommunicator.h"
#include "nlohmann/json.hpp"
#include "GetStateCommand.h"
#include "Logger.h"
#include "ConfigCommand.h"

using json = nlohmann::json;
using namespace std::string_literals;

SilentMonitorCommunicator::SilentMonitorCommunicator(
	std::shared_ptr<BluetoothServer> bluetoothServer, std::shared_ptr<ConfigurationManager> configManager)
	: _bluetoothServer(std::move(bluetoothServer))
	, _configManager(std::move(configManager))
{
}

void SilentMonitorCommunicator::Run() {
	while (!_stopped) {
		std::string request = _bluetoothServer->_receivedCommands.getNext();
		Logger::Info("request got: "s + request);
		auto reqjson = json::parse(request);
		if (reqjson["CommandType"].get<std::string>() == "GetState"s) {
			GetStateCommand cmd;
			_bluetoothServer->SendCommand(cmd.GetResponse());
		}
		if (reqjson["CommandType"].get<std::string>() == "GetConfig"s) {
			ConfigCommand cmd(_configManager);
			_bluetoothServer->SendCommand(cmd.GetResponse());
		}
		if (reqjson["CommandType"].get<std::string>() == "SetConfig"s) {
			ConfigCommand cmd(_configManager);
			_bluetoothServer->SendCommand(
				cmd.SetResponse(
					reqjson["WifiHost"].get<std::string>(),
					reqjson["WifiPort"].get<std::string>()));
		}
	}
}