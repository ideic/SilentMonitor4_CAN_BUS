#pragma once
#include "ConfigurationManager.h"
#include <memory>
#include "TCPClient.h"
#include "ELM327Communicator.h"
#include "Logger/RollingFileLogger.h"
using namespace Xaba;
class CANBUSCommunicator
{
	std::shared_ptr<ConfigurationManager> _configuraionManager;
	std::shared_ptr<TCPClient> _tcpClient;
	std::unique_ptr<ELM327Communicator> _elm327;
	RollingFileLogger _dataLogger;
public:
	CANBUSCommunicator(std::shared_ptr<ConfigurationManager> configManager);
	void Fetch();
};

