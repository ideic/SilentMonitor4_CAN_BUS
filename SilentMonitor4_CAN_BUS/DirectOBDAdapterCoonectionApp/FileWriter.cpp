#include "FileWriter.h"
#include <fstream>
#include <sstream>
struct FileWriter::FileInfo {
	std::ofstream _file;
};
FileWriter::FileWriter(const std::string& fileNameWithPath){
	_fileInfo = std::make_shared<FileWriter::FileInfo>();
	_fileInfo ->_file.open(fileNameWithPath);
	if (!_fileInfo->_file.is_open()) {
		throw std::runtime_error("Log file cannot be opened:" + fileNameWithPath);
	}
}

void FileWriter::Write(const std::vector<uint8_t> &data){
	bool needSeparator{ false };

	for(auto && byteItem : data){
		_fileInfo->_file << std::hex << byteItem;

		if (needSeparator) {
			_fileInfo->_file << ";";
		}
			
		needSeparator = !needSeparator;
	}
	_fileInfo->_file << "\n";

}
