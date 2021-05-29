#pragma once
#include <memory>
#include <atomic>
#include <BlockingQueue.h>
#include "ConfigurationManager.h"
class BluetoothServer 
{
	struct SocketInfo;
	std::shared_ptr<SocketInfo> _socketInfo;
	std::atomic_bool _stopped{ false };
	std::shared_ptr<ConfigurationManager> _configManager;
	std::shared_ptr<void> _configToken{ nullptr };
	void Connect();
public:
	BluetoothServer(std::shared_ptr<ConfigurationManager> configManager);
	~BluetoothServer();
	void Run();
	void Stop();
	void SendCommand(const std::string& command) const;
	Xaba::BlockingQueue<std::string> _receivedCommands;
};

