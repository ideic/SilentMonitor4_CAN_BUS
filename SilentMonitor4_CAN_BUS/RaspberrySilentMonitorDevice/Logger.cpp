#include "Logger.h"
#include <iostream>
using namespace std::string_literals;
void Logger::Info(const std::string &message)
{
	std::cout << message << "\n";
}

void Logger::Error(const std::string &message)
{
	std::cout << "Error: "s << message << "\n";
}