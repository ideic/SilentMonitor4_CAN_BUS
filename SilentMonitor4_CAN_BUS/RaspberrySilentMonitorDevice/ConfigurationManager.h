#pragma once
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <memory>

struct WifiSetting {
	std::string SSID{};
	std::string Host{};
	std::string Port{};
};

enum class LogSinkType
{
	Console,
	File
};
struct LogSetting {
	std::vector<LogSinkType> LogSinks;
};
class ConfigurationManager
{
	WifiSetting _wifiSetting{};
	LogSetting _logSetting{};
	std::string _workingDir{};
	std::string _configFilePath{};
	std::mutex _lock{};
	std::vector<std::weak_ptr<std::function<void()>>> _subscribers;
	void Cleanup();
	bool _isRestartNeeded{ false };
	void ModifySystemWIFIConfig();

public:
	ConfigurationManager();
	WifiSetting GetWifiSetting() const;
	void SetWifiSetting(WifiSetting wifiSetting);

	LogSetting GetLogSetting() const;

	std::string GetWorkingDir() const;

	std::shared_ptr<void>  Subscribe2ConfigStateChange(std::function<void()> subscriber) ;
	static void UnSubscribe(std::shared_ptr<void>  token);

	bool IsRestartNeeded() const;
};

