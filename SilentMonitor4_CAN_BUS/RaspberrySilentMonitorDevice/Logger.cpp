#include "Logger.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "RollingFileLogger.h"
using namespace std::string_literals;
Logger logger;

void Logger::InitLogger(const std::shared_ptr<ConfigurationManager>& configManager) {
	for (auto && sink : configManager->GetLogSetting().LogSinks) {
		if (sink == LogSinkType::Console) {
			_logger._loggerList.emplace_back(new ConsoleLogger());
			continue;
		}
		if (sink == LogSinkType::File) {
			_logger._loggerList.emplace_back(new RollingFileLogger(configManager->GetWorkingDir()));
		}

	}
}

void Logger::Info(const std::string &message)
{
	for (auto & sink : _logger._loggerList) {
		sink->Info(message);
	}
}

void Logger::Error(const std::string &message)
{
	for (auto& sink : _logger._loggerList) {
		sink->Error(message);
	}
}