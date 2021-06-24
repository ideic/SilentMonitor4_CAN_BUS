#pragma once
#include "ILogSink.h"
#include <fstream>
#include <mutex>

namespace Xaba {
	class RollingFileLogger : public ILogSink
	{
		std::string _logFileName;
		int8_t _currentIndex;
		std::ofstream _currentFile{};
		uint16_t _lines;
		void LogMessage(const std::string& message);
		std::mutex _lock;
	public:
		RollingFileLogger(const  std::string& fileNameWithoutExtension);
	
		void Info(const std::string& message) override;
		void Error(const std::string& message) override;

	};
}
