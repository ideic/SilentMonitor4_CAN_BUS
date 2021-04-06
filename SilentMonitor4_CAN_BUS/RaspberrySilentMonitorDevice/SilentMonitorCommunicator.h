#pragma once
#include "BluetoothServer.h"
#include <memory>
#include <atomic>
class SilentMonitorCommunicator
{
	std::shared_ptr<BluetoothServer> _bluetoothServer{};
	std::atomic_bool _stopped{ false };
public:
	SilentMonitorCommunicator(std::shared_ptr<BluetoothServer> bluetoothServer);

	void Run();

};

