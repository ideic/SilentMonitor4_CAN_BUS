#pragma once
#include <memory>
#include <atomic>
#include <BlockingQueue.h>
#include "ConfigurationManager.h"
class BluetoothServer 
{
	struct SocketInfo;
	std::shared_ptr<SocketInfo> _socketInfo;
	std::shared_ptr<ConfigurationManager> _configManager;
	bool _connected;
	void Connect();

public:
	BluetoothServer(std::shared_ptr<ConfigurationManager> configManager);
	~BluetoothServer();

	bool Fetch();
	void SendCommand(const std::string& command) const;
	
	Xaba::BlockingQueue<std::string> _receivedCommands;
};

