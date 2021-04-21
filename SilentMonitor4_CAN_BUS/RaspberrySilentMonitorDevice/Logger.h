#pragma once
#include <string>
#include <vector>
#include "ConfigurationManager.h"
#include <memory>
#include "ILogSink.h"

class Logger
{
	std::vector<std::unique_ptr<ILogSink>> _loggerList{};
public:
	static void InitLogger(const std::shared_ptr<ConfigurationManager>& configManager);
	static void Info(const std::string &message);
	static void Error(const std::string& message);
};

