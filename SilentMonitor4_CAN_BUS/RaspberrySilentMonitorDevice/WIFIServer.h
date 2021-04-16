#pragma once
#include "ConfigurationManager.h"
#include <memory>
#include "IRunable.h"
class WIFIServer : public IRunable
{
	std::shared_ptr<ConfigurationManager> _configuraionManager;
public:
	WIFIServer(std::shared_ptr<ConfigurationManager> configManager);
	void Run() override;
	void Stop() override;
	bool IsRunning() override;
};

