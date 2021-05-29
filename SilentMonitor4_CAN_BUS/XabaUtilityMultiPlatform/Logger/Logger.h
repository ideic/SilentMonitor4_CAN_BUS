#pragma once
#include <string>
#include <vector>
#include <memory>
#include "ILogSink.h"

namespace Xaba {
	enum class LogSinkType :uint8_t
	{
		Console = 1,
		File = 2
	};
	class Logger
	{
		std::vector<std::unique_ptr<ILogSink>> _loggerList{};
	public:
		struct LogContext {
			std::string LogFileName;
		};
		static void InitLogger(const std::vector<LogSinkType> &logsinks, LogContext context);
		static void Info(const std::string& message);
		static void Error(const std::string& message);
	};

}