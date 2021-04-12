#include "SilentMonitorCommunicator.h"

#include "GetStateCommand.h"
#include "Logger.h"
#include "ConfigCommand.h"
#include "JSON.hpp"
#include <string.h>
using Value = Qentem::Value<char>;
namespace JSON = Qentem::JSON;
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
		auto reqjson = JSON::Parse(request.c_str());
		if (strcmp(reqjson["CommandType"].GetString()->Storage(), "GetState") == 0) {
			GetStateCommand cmd;
			_bluetoothServer->SendCommand(cmd.GetResponse());
			continue;
		}
		if (strcmp(reqjson["CommandType"].GetString()->Storage(),"GetConfig") == 0) {
			ConfigCommand cmd(_configManager);
			_bluetoothServer->SendCommand(cmd.GetConfig());
			continue;
		}
		if (strcmp(reqjson["CommandType"].GetString()->Storage(), "SaveConfig") == 0) {
			ConfigCommand cmd(_configManager);
			_bluetoothServer->SendCommand(
				cmd.SetConfig(
					reqjson["WifiHost"].GetString()->Storage(),
					reqjson["WifiPort"].GetString()->Storage()));
			continue;
		}
	}
}