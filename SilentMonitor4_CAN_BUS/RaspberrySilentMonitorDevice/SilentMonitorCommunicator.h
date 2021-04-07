#pragma once
#include "BluetoothServer.h"
#include <memory>
#include <atomic>
#include "ConfigurationManager.h"

class SilentMonitorCommunicator
{
	std::shared_ptr<BluetoothServer> _bluetoothServer{};
	std::shared_ptr<ConfigurationManager> _configManager{};

	std::atomic_bool _stopped{ false };
public:
	SilentMonitorCommunicator(std::shared_ptr<BluetoothServer> bluetoothServer, std::shared_ptr<ConfigurationManager> configManager);

	void Run();

};

