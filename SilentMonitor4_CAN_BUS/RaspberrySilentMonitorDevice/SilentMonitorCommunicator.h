#pragma once
#include "BluetoothServer.h"
#include <memory>
#include <atomic>
#include "ConfigurationManager.h"
#include <CANBUSCommunicator.h>
class SilentMonitorCommunicator
{
	std::shared_ptr<BluetoothServer> _bluetoothServer{};
	std::shared_ptr<ConfigurationManager> _configManager{};
	std::shared_ptr<CANBUSCommunicator> _canbusCommunicator{};
	std::shared_ptr<void> _configSubscribeToken;
	std::atomic_bool _stopped{ false };
public:
	SilentMonitorCommunicator(std::shared_ptr<BluetoothServer> bluetoothServer, std::shared_ptr<ConfigurationManager> configManager, std::shared_ptr<CANBUSCommunicator> canbusCommunicator);

	void Run();
	void Stop();
};

