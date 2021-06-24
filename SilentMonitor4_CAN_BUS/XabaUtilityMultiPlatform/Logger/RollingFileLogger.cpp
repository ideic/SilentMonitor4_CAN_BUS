#include "RollingFileLogger.h"

#include <string>
#include <thread>
#include <sstream>
#include <DateTimeProvider.h>
#ifdef WIN32
	#include <filesystem>
	#define FILE_EXIST std::filesystem::exists

#endif // WIN32
#ifndef WIN32
	#include <sys/stat.h>
	
	inline bool file_exists(const std::string& name) {
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}

	#define FILE_EXIST file_exists
#endif
using namespace std::string_literals;
using namespace Xaba;
constexpr uint8_t MAX_FILES = 5;
constexpr  uint16_t MAX_LINES = 10'000;


RollingFileLogger::RollingFileLogger(const  std::string& fileNameWithoutExtension):_logFileName(fileNameWithoutExtension), _lines(0) {
	_currentIndex = -1;
	std::string fileName;
	for (auto i = 0; i < MAX_FILES; ++i) {
		fileName =   _logFileName + std::to_string(i) + ".log"s;
		if (FILE_EXIST(fileName)) {
			continue;
		}

		_currentIndex = i;
		break;
	}
	if (_currentIndex == -1) {
		_currentIndex = 0;
		fileName = _logFileName + std::to_string(_currentIndex) + ".log"s;
	}

	_currentFile.open(fileName);
	if (!_currentFile.is_open()) {
		throw std::runtime_error("Log file cannot be opened:" + fileName);
	}

}


void RollingFileLogger::LogMessage(const std::string& message)
{
	std::lock_guard l(_lock);
	_currentFile << message << std::endl;
	if (++_lines >= MAX_LINES) {
		++_currentIndex;
		if (_currentIndex >= MAX_FILES) _currentIndex = 0;
		_currentFile.close();
		auto fileName = _logFileName + std::to_string(_currentIndex) + ".log"s;

		_currentFile.open(fileName);
		if (!_currentFile.is_open()) {
			throw std::runtime_error("Log file cannot be opened:" + fileName);
		}
	}
}

void RollingFileLogger::Info(const std::string& message) {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	LogMessage(DateTimeProvider::DateTme()->to_string() + " ["s + ss.str() + "] [INFO] "s + message);
}

void RollingFileLogger::Error(const std::string& message)
{
	std::stringstream ss;
	ss << std::this_thread::get_id();
	LogMessage(DateTimeProvider::DateTme()->to_string() + " ["s + ss.str() + "] [ERROR] "s + message);
}
