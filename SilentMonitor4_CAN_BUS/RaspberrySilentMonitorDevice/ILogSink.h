#pragma once
#include <string>
class ILogSink
{
public:
	virtual ~ILogSink() = default;
	virtual void Info(const std::string &message) = 0;
	virtual void Error(const std::string& message) = 0;
};

