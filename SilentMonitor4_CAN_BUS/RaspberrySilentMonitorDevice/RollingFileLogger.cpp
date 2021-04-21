#include "RollingFileLogger.h"
#include <filesystem>
#include <string>
#include <thread>
#include <sstream>
#include <sys/time.h>
#include <sys/stat.h>

using namespace std::string_literals;

constexpr uint8_t MAX_FILES = 5;
const std::string LOGFILENAME = "canbusniffer_";
constexpr  uint16_t MAX_LINES = 10'000;

inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

RollingFileLogger::RollingFileLogger(std::string workingDir): _workingDir(std::move(workingDir)), _lines(0) {
	_currentIndex = std::nullopt;
	std::string fileName;
	for (auto i = 0; i < MAX_FILES; ++i) {
		fileName =  _workingDir + LOGFILENAME + std::to_string(i) + ".log"s;
		if (file_exists(fileName)) {
			continue;
		}

		_currentIndex = i;
		break;
	}
	if (!_currentIndex.has_value()) {
		fileName = _workingDir + LOGFILENAME + std::to_string(0) + ".log"s;
	}

	_currentFile.open(fileName);
	if (!_currentFile.is_open()) {
		throw std::runtime_error("Log file cannot be opened:" + fileName);
	}

}

RollingFileLogger::~RollingFileLogger() = default;

void RollingFileLogger::LogMessage(const std::string& message)
{
	std::lock_guard l(_lock);
	_currentFile << message << std::endl;
	if (++_lines >= MAX_LINES) {
		_currentIndex = _currentIndex.value() + 1;
		if (_currentIndex >= MAX_FILES) _currentIndex = 0;
		_currentFile.close();
		auto fileName = _workingDir + LOGFILENAME + std::to_string(_currentIndex.value()) + ".log"s;

		_currentFile.open(fileName);
		if (!_currentFile.is_open()) {
			throw std::runtime_error("Log file cannot be opened:" + fileName);
		}
	}
}
std::string RollingFileLogger::GetDateTime()
{
	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;

	char buffer[80];
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));

	char currentTime[84] = "";
	sprintf(currentTime, "%s:%03d", buffer, milli);

	return currentTime;
}
void RollingFileLogger::Info(const std::string& message) {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	LogMessage(GetDateTime() + " ["s + ss.str() + "] [INFO] "s + message);
}

void RollingFileLogger::Error(const std::string& message)
{
	std::stringstream ss;
	ss << std::this_thread::get_id();
	LogMessage(GetDateTime() + " ["s + ss.str() + "] [ERROR] "s + message);
}
