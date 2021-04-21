#include "ConsoleLogger.h"
#include <iostream>

void ConsoleLogger::Info(const std::string& message) {
	std::cout << message << '\n';
}

void ConsoleLogger::Error(const std::string& message) {
	std::cerr << message << '\n';
}
