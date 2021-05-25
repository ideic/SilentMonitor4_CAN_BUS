// DirectOBDAdapterCoonectionApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma comment( lib, "Ws2_32.lib" )

#include <iostream>
#include <vector>
#include <TCPClient.h>
#include "DirectOBDAdapterCoonectionApp.h"
#include "FileWriter.h"
#include <ELM327Communicator.h>
#include <DateTimeProvider.h>
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
void TerminalMode(const std::string& host, const std::string& port) {
	TCPClient tcp(host, port);

	while (true) {
		try {
			std::string input;
			std::cout << "Command ?:" << std::endl;
			std::getline(std::cin, input);
			tcp.SendData(input + "\r"s);
			auto data = tcp.ReceiveData();
			for (auto& charItem : data)
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

int main(int argc, char* argv[])
{
    std::cout << "Hello!\n";
	for (int i = 0; i < argc; i++)
		std::cout << "Param" << i << ":" << argv[i] << std::endl;

	InputParser parser(argc, argv);

	// Validate the parameters
	if (argc == 1) {
		std::cout << "usage: --host [HOST IP] --port [PORT] --terminal\n";
		return 1;
	}

	std::string host = parser.getCmdOption("--host");
	std::string port = parser.getCmdOption("--port");
	auto terminalMode = parser.cmdOptionExists("--terminal");

	if (terminalMode) {
		TerminalMode(host, port);
		return 0;
	}

	FileWriter writer("receivedata.log");

	auto tcpClient = std::make_shared<TCPClient>(host, port);
	ELM327Communicator communicator(tcpClient);
	communicator.Connect();
	while (true) {
		try {
			for (auto [code, value] : communicator.ReadLiveCodeValues()) {
				std::cout << DateTimeProvider::DateTme()->UtcNow()<< " " << code << " "s << value << std::endl;
			}
			auto [voltage, value] = communicator.ReadVoltageCodeValue();
			std::cout << DateTimeProvider::DateTme()->UtcNow()<< " "<< voltage << " "s << value << std::endl;

			for (auto [errorcode, errorvalue] : communicator.ReadErrorCodeValues()) {
				std::cerr << DateTimeProvider::DateTme()->UtcNow() << " " << errorcode << " "s << errorvalue << std::endl;
			}
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			break;
		}
	}
	return 0;
}