#include "Logger.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "RollingFileLogger.h"
using namespace std::string_literals;
using namespace Xaba;
static Logger _logger;
void Logger::InitLogger(const std::vector<LogSinkType> &logsinks, LogContext context) {

	for (auto& logsink : logsinks) {
		if (logsink == LogSinkType::Console)
			_logger._loggerList.emplace_back(new ConsoleLogger());

		if (logsink == LogSinkType::File)
			_logger._loggerList.emplace_back(new RollingFileLogger(context.LogFileName));
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