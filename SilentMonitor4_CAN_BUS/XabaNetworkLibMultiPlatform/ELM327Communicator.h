#pragma once
#include <memory>
#include "TCPClient.h"
#include <vector>
using namespace std::string_literals;

class ELM327Communicator
{
	class CodeEntity {
		std::string _sentCode; 
		std::string _receivedCode;
	public:
		CodeEntity(std::string sentCode, std::string receivedCode);
		std::string ResponseValue();
		CodeEntity Log();
	};

	uint8_t _protocol{0};
	std::shared_ptr<TCPClient> _networkHandler;
	std::vector<std::string> _supportedPins;
	
	CodeEntity SendCode(std::string code);
	void GetNextProtocol(uint16_t& protocolNumber);
	void SetSupportedPID(std::string_view supportedPINSAnswer, uint16_t offset);
	void SetSupportedPIDs(std::vector<std::string> pidQueryCodes);
public:
	ELM327Communicator(std::shared_ptr<TCPClient> networkHandler);
	void Connect();
	std::vector<std::pair<std::string, std::string>> ReadLiveCodesValues();

};

