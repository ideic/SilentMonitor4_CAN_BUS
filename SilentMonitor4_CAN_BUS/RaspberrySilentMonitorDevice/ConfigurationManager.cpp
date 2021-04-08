#include "ConfigurationManager.h"
#include "nlohmann/json.hpp"
#include <unistd.h>

using json = nlohmann::json;

using namespace std::string_literals;

ConfigurationManager::ConfigurationManager() {
    std::string currentPath;
	char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        currentPath = cwd;
    }
    const std::string configFilePath = currentPath.append("/canbussniffer.config"s);

   _config.open(configFilePath);
   if (!_config.is_open()) {
       _workingDir = currentPath;
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
           const std::string sink = logsinks.substr(0, logsinks.find_first_of(';'));
           if (sink == "console"s)
               _logSetting.LogSinks.push_back(LogSink::Console);
           else
               _logSetting.LogSinks.push_back(LogSink::File);

           logsinks = logsinks.substr(logsinks.find_first_of(';') + 1);
       }
       
   }

}
WifiSetting ConfigurationManager::GetWifiSetting() const {
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

LogSetting ConfigurationManager::GetLogSetting() const {
    return _logSetting;
}

std::string ConfigurationManager::GetWorkingDir() const {
    return _workingDir;
}
