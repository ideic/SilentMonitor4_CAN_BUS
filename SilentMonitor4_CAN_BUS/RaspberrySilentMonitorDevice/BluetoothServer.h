#pragma once
#include <string>
#include <memory>
#include <atomic>
#include "BlockingQueue.h"
struct BluetoothServer
{
	class SocketInfo;
	std::shared_ptr<SocketInfo> _socketInfo;
	std::atomic_bool _stopped{ false };
	void Connect();
public:
	BluetoothServer(std::string address);
	~BluetoothServer();
	void Run();
	void Stop();
	void SendCommand(const std::string& command);
	BlockingQueue<std::string> _receivedCommands;
};

