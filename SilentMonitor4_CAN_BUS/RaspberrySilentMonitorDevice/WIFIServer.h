#pragma once
#include "ConfigurationManager.h"
#include <memory>
class WIFIServer
{
	std::shared_ptr<ConfigurationManager> _configuraionManager;
public:
	WIFIServer(std::shared_ptr<ConfigurationManager> configManager);
	void Run();
	void Stop();

};

