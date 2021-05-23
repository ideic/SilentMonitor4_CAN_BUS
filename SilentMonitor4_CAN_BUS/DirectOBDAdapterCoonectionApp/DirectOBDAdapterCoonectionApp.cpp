// DirectOBDAdapterCoonectionApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma comment( lib, "Ws2_32.lib" )

#include <iostream>
#include <vector>
#include <TCPClient.h>
#include "DirectOBDAdapterCoonectionApp.h"
#include <DirectOBDAdapterCoonectionApp/FileWriter.h>

using namespace std::string_literals;

class InputParser {
public:
	InputParser(int& argc, char** argv) {
		for (int i = 1; i < argc; ++i)
			this->tokens.push_back(std::string(argv[i]));
	}
	const std::string& getCmdOption(const std::string& option) const {
		std::vector<std::string>::const_iterator itr;
		itr = std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}
	bool cmdOptionExists(const std::string& option) const {
		return std::find(this->tokens.begin(), this->tokens.end(), option)
			!= this->tokens.end();
	}
private:
	std::vector <std::string> tokens;
};

int main(int argc, char* argv[])
{
    std::cout << "Hello!\n";
	for (int i = 0; i < argc; i++)
		std::cout << "Param" << i << ":" << argv[i] << std::endl;

	InputParser parser(argc, argv);

	// Validate the parameters
	if (argc != 5) {
		std::cout << "usage: --host [HOST IP] --port [PORT] \n";
		return 1;
	}

	std::string host = parser.getCmdOption("--host");
	std::string port = parser.getCmdOption("--port");

	TCPClient tcp(host, port);
	FileWriter writer("receivedata.log");
	tcp.Start();

	while (true) {
		try {
			std::string input;
			std::cout << "Command ?:" << std::endl;
			std::getline(std::cin, input);
			tcp.SendData(input + "\r"s);
			auto data = tcp.ReceiveData();
			for (auto &charItem : data)
			{
				if (charItem == 0xd || charItem == 0x3e) {
					charItem = 0x20;
				}
			}
			std::cout << std::string((char*)data.data(), data.size()) << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			break;
		}
	}

}
