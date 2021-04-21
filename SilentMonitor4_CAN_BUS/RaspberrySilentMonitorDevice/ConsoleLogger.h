#pragma once
#include "ILogSink.h"

class ConsoleLogger: public ILogSink
{
public:
	void Info(const std::string& message) override;
	void Error(const std::string& message) override;
};

