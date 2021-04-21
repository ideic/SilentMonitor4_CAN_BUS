#include "RollingFileLogger.h"
#include <filesystem>
#include <string>
using namespace std::string_literals;

constexpr uint8_t MAX_FILES = 5;
const std::string LOGFILENAME = "canbusniffer_";
constexpr  uint16_t MAX_LINES = 10'000;

RollingFileLogger::RollingFileLogger(std::string workingDir): _workingDir(std::move(workingDir)), _lines(0) {
	_currentIndex = std::nullopt;
	std::string fileName;
	for (auto i = 0; i < MAX_FILES; ++i) {
		fileName =  _workingDir + LOGFILENAME + std::to_string(i) + ".log"s;
		if (std::filesystem::exists(fileName)) {
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

void RollingFileLogger::Info(const std::string& message) {
	_currentFile << message <<"\n";
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
