#pragma once
#include "StateCommandBase.h"
#include "ConfigurationManager.h"
#include <memory>
class ConfigCommand: public StateCommandBase
{
	std::shared_ptr<ConfigurationManager> _configManager;
public:
	ConfigCommand(std::shared_ptr<ConfigurationManager> configManager);
	std::string GetResponse();
	std::string SetResponse(const std::string &host, const std::string &port);
};

