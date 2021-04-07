#pragma once
#include <string>
#include <memory>
#include <atomic>
#include "BlockingQueue.h"
struct BluetoothServer
{
	struct SocketInfo;
	std::shared_ptr<SocketInfo> _socketInfo;
	std::atomic_bool _stopped{ false };
	void Connect();
public:
	BluetoothServer();
	~BluetoothServer();
	void Run();
	void Stop();
	void SendCommand(const std::string& command) const;
	BlockingQueue<std::string> _receivedCommands;
};
