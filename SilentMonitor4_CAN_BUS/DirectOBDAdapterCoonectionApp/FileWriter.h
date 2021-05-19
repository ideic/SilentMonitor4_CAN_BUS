#pragma once
#include <string>
#include <vector>
#include <memory>
class FileWriter
{
	struct FileInfo;
	std::shared_ptr<FileInfo> _fileInfo;
public:
	FileWriter(const std::string& fileNameWithPath);
	void Write(const std::vector<uint8_t> &data);
};

