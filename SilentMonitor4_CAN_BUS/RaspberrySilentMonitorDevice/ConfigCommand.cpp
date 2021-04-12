#include "ConfigCommand.h"
#include "ConfigurationManager.h"
#include "JSON.hpp"
#include <sstream>

using Value = Qentem::Value<char>;

ConfigCommand::ConfigCommand(std::shared_ptr<ConfigurationManager> configManager): _configManager(std::move(configManager))
{
}

std::string ConfigCommand::GetConfig()
{
	Value v;
	v["Status"] = "OK";
	v["WifiHost"] = _configManager->GetWifiSetting().Host.c_str();
	v["WifiPort"] = _configManager->GetWifiSetting().Port.c_str();
	v["ErrorMessage"] =  "";
	
	std::stringstream osstream;
	osstream << v.Stringify();
	return StateCommandBase::GetResponse(osstream.str());
}

std::string ConfigCommand::SetConfig(const std::string& host, const std::string& port)
{
	Value v;
	try
	{
		_configManager->SetWifiSetting(WifiSetting{ host, port });
		v["Status"] = "OK";
		v["ErrorMessage"] = "";

	}
	catch (const std::exception& ex)
	{
		v["Status"] = "Failed";
		v["ErrorMessage"] = ex.what();
	}

	std::stringstream osstream;
	osstream << v.Stringify();
	return StateCommandBase::GetResponse(osstream.str());
}
