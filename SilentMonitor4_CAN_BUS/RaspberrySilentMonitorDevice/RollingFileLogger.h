#pragma once
#include "ILogSink.h"
#include "ConfigurationManager.h"
#include <optional>
#include <fstream>

class RollingFileLogger: public ILogSink
{	
	std::string _workingDir;
	std::optional<uint8_t> _currentIndex;
	std::ofstream _currentFile{};
	uint16_t _lines;
public:
	RollingFileLogger(std::string workingDir);
	~RollingFileLogger();
	void Info(const std::string& message) override;
	void Error(const std::string& message) override;
	
};

