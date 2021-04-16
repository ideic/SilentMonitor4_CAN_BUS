#pragma once
#include <memory>
#include <atomic>
#include "BlockingQueue.h"
#include "ConfigurationManager.h"
#include "IRunable.h"
class BluetoothServer : public IRunable
{
	struct SocketInfo;
	std::shared_ptr<SocketInfo> _socketInfo;
	std::atomic_bool _stopped{ false };
	std::shared_ptr<ConfigurationManager> _configManager;
	std::shared_ptr<void> _configToken;
	void Connect();
public:
	BluetoothServer(std::shared_ptr<ConfigurationManager> configManager);
	~BluetoothServer();
	void Run() override;
	void Stop() override;
	bool IsRunning() override;
	void SendCommand(const std::string& command) const;
	BlockingQueue<std::string> _receivedCommands;
};

