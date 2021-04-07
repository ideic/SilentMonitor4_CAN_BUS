#include "ConfigCommand.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

ConfigCommand::ConfigCommand(std::shared_ptr<ConfigurationManager> configManager): _configManager(std::move(configManager))
{
}

std::string ConfigCommand::GetResponse()
{
	json j;
	j["Status"] = "OK";
	j["WifiHost"] = _configManager->GetWifiSetting().Host;
	j["WifiPort"] = _configManager->GetWifiSetting().Port;
	j["ErrorMessage"] = "";
	return StateCommandBase::GetResponse(j.dump());
}

std::string ConfigCommand::SetResponse(const std::string& host, const std::string& port)
{
	json j;
	try
	{
		_configManager->SetWifiSetting(WifiSetting{ host, port });
		j["Status"] = "OK";
		j["ErrorMessage"] = "";
	}
	catch (const std::exception& ex)
	{
		j["Status"] = "Failed";
		j["ErrorMessage"] = ex.what();
	}
	
	return StateCommandBase::GetResponse(j.dump());
}
