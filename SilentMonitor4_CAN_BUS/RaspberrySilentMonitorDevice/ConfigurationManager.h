#pragma once
#include <string>
#include <vector>
#include <fstream>

struct WifiSetting {
	std::string Host{};
	std::string Port{};
};

enum class LogSink
{
	Console,
	File
};
struct LogSetting {
	std::vector<LogSink> LogSinks;
};
class ConfigurationManager
{
	std::fstream _config{};
	WifiSetting _wifiSetting{};
	LogSetting _logSetting{};
	std::string _workingDir{};
public:
	ConfigurationManager();
	WifiSetting GetWifiSetting() const;
	void SetWifiSetting(WifiSetting wifiSetting);

	LogSetting GetLogSetting() const;

	std::string GetWorkingDir() const;
};

