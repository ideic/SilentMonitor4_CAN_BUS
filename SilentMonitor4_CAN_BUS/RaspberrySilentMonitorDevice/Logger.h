#pragma once
#include <string>
class Logger
{public:
	static void Info(const std::string &message);
	static void Error(const std::string& message);
};

