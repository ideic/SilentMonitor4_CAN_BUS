#include "ConsoleLogger.h"
#include <iostream>
#include <thread>
#include <sstream>
#include "../DateTimeProvider.h"

using namespace Xaba;
using namespace std::string_literals;

void ConsoleLogger::Info(const std::string& message) {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::cout << DateTimeProvider::DateTme()->to_string() << " ["s << ss.str() << "] [INFO] "s << message << "\n";
}

void ConsoleLogger::Error(const std::string& message) {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::cerr << DateTimeProvider::DateTme()->to_string()<< " ["s << ss.str() << "] [ERROR] "s << message << "\n";
}
