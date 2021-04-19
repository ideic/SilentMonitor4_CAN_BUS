#pragma once
#include "StateCommandBase.h"
#include <memory>

class ConfigurationManager;

class ConfigCommand: public StateCommandBase
{
	std::shared_ptr<ConfigurationManager> _configManager;
public:
	ConfigCommand(std::shared_ptr<ConfigurationManager> configManager);
	std::string GetConfig();
	std::string SetConfig(const std::string& ssid, const std::string &host, const std::string &port);
};

