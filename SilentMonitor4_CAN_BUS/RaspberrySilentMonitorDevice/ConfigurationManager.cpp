#include "ConfigurationManager.h"
#include "JSON.hpp"
#include <unistd.h>
#include <sstream>

using Value = Qentem::Value<char>;
namespace JSON= Qentem::JSON;
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
    _config.write(contentString.c_str(), contentString.size());
}

LogSetting ConfigurationManager::GetLogSetting() const {
    return _logSetting;
}

std::string ConfigurationManager::GetWorkingDir() const {
    return _workingDir;
}
