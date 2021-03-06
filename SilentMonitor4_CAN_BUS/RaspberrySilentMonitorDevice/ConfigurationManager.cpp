#include "ConfigurationManager.h"
#include "JSON.hpp"
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <memory>
#include <algorithm>
#include <utility>
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
       _logSetting.LogSinks.push_back(LogSinkType::Console);
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
       _wifiSetting.SSID = config["WifiSSID"].GetString()->Storage();
       _wifiSetting.Port = config["WifiPort"].GetString()->Storage();
       _workingDir = config["WorkingDir"].GetString()->Storage();
       std::string logsinks = config["LogSinks"].GetString()->Storage();
       while (logsinks.length() > 0) {
           const std::string sink = logsinks.substr(0, logsinks.find_first_of(';'));
           if (sink == "console"s)
               _logSetting.LogSinks.push_back(LogSinkType::Console);
           else
               _logSetting.LogSinks.push_back(LogSinkType::File);

           logsinks = logsinks.substr(logsinks.find_first_of(';') + 1);
       }
       
   }

}
WifiSetting ConfigurationManager::GetWifiSetting() const {
    return _wifiSetting;
}

void ConfigurationManager::SetWifiSetting(WifiSetting wifiSetting)
{
    _isRestartNeeded = _wifiSetting.SSID != wifiSetting.SSID;
	
    _wifiSetting = std::move(wifiSetting);
    Value content;
    
    content["WifiSSID"] = _wifiSetting.SSID.c_str();
    content["WifiHost"] = _wifiSetting.Host.c_str();
    content["WifiPort"] = _wifiSetting.Port.c_str();
    content["WorkingDir"] = _workingDir.c_str();
    std::string sinks;
    for (auto&& sink : _logSetting.LogSinks) {
        sinks += (sink == LogSinkType::Console ? "console" : "file") + ";"s;
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

    if (_isRestartNeeded) {
        ModifySystemWIFIConfig();
    }
	
    for (auto && subscriber : _subscribers) {
        auto sub = subscriber.lock();
    	if (sub)
			(*sub)();
    }
}
void ConfigurationManager::ModifySystemWIFIConfig() {
    std::string configTemplate = R"(
# interfaces(5) file used by ifup(8) and ifdown(8)

# Please note that this file is written to be used with dhcpcd
# For static IP, consult /etc/dhcpcd.conf and 'man dhcpcd.conf'

# Include files from /etc/network/interfaces.d:
source-directory /etc/network/interfaces.d
auto wlan0
iface wlan0 inet dhcp
                wpa-ssid "#SSID#"
#               wpa-psk ""
auto eth0
iface eth0 inet dhcp
)";
    std::ofstream config{};
    config.open("/etc/network/interfaces", std::ios_base::trunc);
    if (!config.is_open()) {
        char err[1024];
        perror(err);
        throw std::runtime_error("Error cannot open config file /etc/network/interfaces error: "s + std::string(err));
    }
    
    config << configTemplate.replace(configTemplate.find("#SSID#"s), "#SSID#"s.size(), _wifiSetting.SSID);
    config.close();
}

LogSetting ConfigurationManager::GetLogSetting() const {
    return _logSetting;
}

std::string ConfigurationManager::GetWorkingDir() const {
    return _workingDir;
}

std::shared_ptr<void> ConfigurationManager::Subscribe2ConfigStateChange(std::function<void()> subscriber) {
    std::lock_guard l(_lock);
    std::shared_ptr<std::function<void()>> result(new std::function<void()>(std::move(subscriber)), [&](std::function<void()>* s) { delete s; Cleanup(); });
    _subscribers.push_back(result);

    return result;
}

void ConfigurationManager::UnSubscribe(std::shared_ptr<void> token) {
    token.reset();
}

bool ConfigurationManager::IsRestartNeeded() const {
    return _isRestartNeeded;
}

void ConfigurationManager::Cleanup()
{
    _subscribers.erase(
        std::remove_if(_subscribers.begin(), _subscribers.end(),
            [](std::weak_ptr<std::function<void()>> t) { return t.expired(); }
        ),
        _subscribers.end()
    );
}
