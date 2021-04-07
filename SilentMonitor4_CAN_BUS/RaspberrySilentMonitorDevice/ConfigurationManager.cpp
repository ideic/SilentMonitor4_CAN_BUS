#include "ConfigurationManager.h"
#include "nlohmann/json.hpp"
#include <filesystem>
using json = nlohmann::json;

using namespace std::string_literals;

ConfigurationManager::ConfigurationManager() {
    auto currentpath = std::filesystem::current_path();
    auto configFilePath = currentpath.generic_string() + "/canbussniffer.config"s;

   _config.open(configFilePath);
   if (!_config.is_open()) {
       _workingDir = std::filesystem::current_path().generic_string();
   }
   else {
       std::string content;
       std::string line;
       while (getline(_config, line))
       {
           content += line;
       }

       auto config = json::parse(content);
       _wifiSetting.Host = config["WifiHost"].get<std::string>();
       _wifiSetting.Port = config["WifiPort"].get<std::string>();
       _workingDir = config["WorkingDir"].get<std::string>();
       auto logsinks = config["LogSinks"].get<std::string>();
       while (logsinks.length() > 0) {
           auto sink = logsinks.substr(0, logsinks.find_first_of(';'));
           if (sink == "console")
               _logSetting.LogSinks.push_back(LogSink::Console);
           else
               _logSetting.LogSinks.push_back(LogSink::File);

           logsinks = logsinks.substr(logsinks.find_first_of(';') + 1);
       }
       
   }

}
WifiSetting ConfigurationManager::GetWifiSetting()
{
    return _wifiSetting;
}

void ConfigurationManager::SetWifiSetting(WifiSetting wifiSetting)
{
    _wifiSetting = wifiSetting;
    json content;
    content["WifiHost"] = _wifiSetting.Host;
    content["WifiPort"] = _wifiSetting.Port;
    content["WorkingDir"] = _workingDir;
    std::string sinks;
    for (auto&& sink : _logSetting.LogSinks) {
        sinks += (sink == LogSink::Console ? "console" : "file") + ";"s;
    }

    content["LogSinks"] = sinks;
    auto contentString = content.dump();
    _config.write(contentString.c_str(), contentString.size());
}

LogSetting ConfigurationManager::GetLogSetting()
{
    return _logSetting;
}

std::string ConfigurationManager::GetWorkingDir()
{
    return _workingDir;
}
