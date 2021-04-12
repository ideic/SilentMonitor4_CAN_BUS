#include "ConfigurationManager.h"
#include "JSON.hpp"
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <stdio.h>

using Value = Qentem::Value<char>;
namespace JSON= Qentem::JSON;
using namespace std::string_literals;

ConfigurationManager::ConfigurationManager() {
    std::string currentPath;
	char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        currentPath = cwd;
    }
    _configFilePath = currentPath.append("/canbussniffer.config"s);

   std::ifstream config{};
   config.open(_configFilePath);
   if (!config.is_open()) {
       _workingDir = currentPath;
   }
   else {
       std::string content;
       std::string line;
       while (getline(config, line))
       {
           content += line;
       }

       auto config = JSON::Parse(content.c_str());
       _wifiSetting.Host = config["WifiHost"].GetString()->Storage();
       _wifiSetting.Port = config["WifiPort"].GetString()->Storage();
       _workingDir = config["WorkingDir"].GetString()->Storage();
       std::string logsinks = config["LogSinks"].GetString()->Storage();
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
    Value content;
 
    content["WifiHost"] = _wifiSetting.Host.c_str();
    content["WifiPort"] = _wifiSetting.Port.c_str();
    content["WorkingDir"] = _workingDir.c_str();
    std::string sinks;
    for (auto&& sink : _logSetting.LogSinks) {
        sinks += (sink == LogSink::Console ? "console" : "file") + ";"s;
    }

    content["LogSinks"] = sinks.c_str();
	
    std::stringstream osstream;
    osstream << content.Stringify();
	
    std::string contentString = osstream.str();

    std::ofstream config{};
    config.open(_configFilePath, std::ios_base::trunc);
    if (!config.is_open()) {
        char err[1024];
        perror(err);
        throw std::runtime_error("Error cannot open config file"s + std::string(err));
    }
    config << contentString;
    config.close();
}

LogSetting ConfigurationManager::GetLogSetting() const {
    return _logSetting;
}

std::string ConfigurationManager::GetWorkingDir() const {
    return _workingDir;
}
