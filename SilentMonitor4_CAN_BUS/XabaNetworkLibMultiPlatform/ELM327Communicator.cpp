#include "ELM327Communicator.h"
#include  <iomanip>
struct ATCommands {
	static const std::string ATI_DisplayId;
	static const std::string ATD_SetBackDefaults;
	static const std::string ATZ_ResetAll;
	static const std::string ATE1_EchoOn;
	static const std::string ATL0_LineFeedOff;
	static const std::string ATDP_DisplayProtocol;
	static const std::string ATSPX_SetProtocolToX;
	static const std::string ATIB96_9600BaudRate;
	static const std::string ATIB48_4800BaudRate;

};

const std::string ATCommands::ATI_DisplayId = "ATI"s;
const std::string ATCommands::ATD_SetBackDefaults = "ATD"s;
const std::string ATCommands::ATZ_ResetAll = "ATZ"s;

const std::string ATCommands::ATE1_EchoOn = "ATE1"s;
const std::string ATCommands::ATL0_LineFeedOff = "ATL0";
const std::string ATCommands::ATDP_DisplayProtocol = "ATDP"s;
const std::string ATCommands::ATSPX_SetProtocolToX = "ATSP"s;
const std::string ATCommands::ATIB96_9600BaudRate = "ATIB96"s;
const std::string ATCommands::ATIB48_4800BaudRate = "ATIB48"s;


struct OBDCommands {
	static const std::string Code_01_ShowCurrentData;
};
const std::string OBDCommands::Code_01_ShowCurrentData = "01"s;

struct OBDPIDs {
	static const std::string Code_00_SupportedPIDS;
};
const std::string OBDPIDs::Code_00_SupportedPIDS = "00"s;


enum class OBDProtocols :uint8_t {
	Automatic = 0,
	SAE_J1850_PWM = 1,
	SAE_J1850_VPW = 2,
	ISO_9141_2 = 3,
	ISO_14230_4_KWP_5baud = 4,
	ISO_14230_4_KWP_FAST_INIT = 5,
	ISO_15765_4_CAN_11bit_500kbaud = 6,
	ISO_15765_4_CAN_29bit_500kbaud = 7,
	ISO_15765_4_CAN_11bit_250kbaud = 8,
	ISO_15765_4_CAN_29bit_250kbaud = 9,
	SAE_J1939_CAN = 0x0A,
	END = 0x0B
};

ELM327Communicator::CodeEntity::CodeEntity(std::string sentCode, std::string receivedCode) : 
	_sentCode(std::move(sentCode)), _receivedCode(std::move(receivedCode)){

}

std::string ELM327Communicator::CodeEntity::ResponseValue() {
	return _receivedCode;
}

ELM327Communicator::CodeEntity ELM327Communicator::CodeEntity::Log() {
	
	return *this;
}

ELM327Communicator::ELM327Communicator(std::shared_ptr<TCPClient> networkHandler): _networkHandler(networkHandler){
}

ELM327Communicator::CodeEntity ELM327Communicator::SendCode (std::string code){
	_networkHandler->SendData(code +"\r"s);
	auto response =_networkHandler->ReceiveData();
	
	for (auto& charItem : response)
	{
		if (charItem == 0xd || charItem == 0x3e) {
			charItem = 0x20;
		}
	}

	return CodeEntity(code, std::string((char*)response.data(), response.size()));

}
void ELM327Communicator::GetNextProtocol(uint16_t& protcolNumber) {
	++protcolNumber;

	if (protcolNumber == (uint8_t)OBDProtocols::END) {
		throw std::runtime_error("There is no more protocol is available.");
	}
}
void ELM327Communicator::Connect(){
	//Reset
	SendCode(ATCommands::ATI_DisplayId).Log();
	SendCode(ATCommands::ATD_SetBackDefaults);
	SendCode(ATCommands::ATZ_ResetAll).Log();

	//Setting
	SendCode(ATCommands::ATE1_EchoOn);
	SendCode(ATCommands::ATL0_LineFeedOff);
	
	//Find Protocoll
	bool protocolNotFound = true;
	uint16_t protocolNumber = 0;
	uint8_t retry{ 0 };
	std::string supportedPINs;
	while (protocolNotFound) {
		try {
			std::stringstream protcolCode;
			protcolCode << std::hex << protocolNumber;
			
			auto response = SendCode(ATCommands::ATSPX_SetProtocolToX + protcolCode.str()).Log().ResponseValue();
			if (response.find("ERROR"s) != std::string::npos) {
				GetNextProtocol(protocolNumber);
				retry = 0;
				continue;
			}

			response = SendCode(OBDCommands::Code_01_ShowCurrentData+ OBDPIDs::Code_00_SupportedPIDS).Log().ResponseValue();
			bool found = response.find("NODATA"s) != std::string::npos;
			if (!found) {
				protocolNotFound = false;
				SendCode(ATCommands::ATDP_DisplayProtocol).Log();
				supportedPINs = response;
				_protocol = protocolNumber;
				continue;
			}

			if (protocolNumber == (uint8_t)OBDProtocols::ISO_9141_2
				|| protocolNumber == (uint8_t)OBDProtocols::ISO_14230_4_KWP_5baud
				|| protocolNumber == (uint8_t)OBDProtocols::ISO_14230_4_KWP_FAST_INIT
				) {
		
				SendCode(ATCommands::ATIB96_9600BaudRate).Log();
				response = SendCode(OBDCommands::Code_01_ShowCurrentData + OBDPIDs::Code_00_SupportedPIDS).Log().ResponseValue();
				found = response.find("NODATA"s) != std::string::npos;
				if (!found) {
					protocolNotFound = false;
					SendCode(ATCommands::ATDP_DisplayProtocol).Log();
					supportedPINs = response;
					_protocol = protocolNumber;
					continue;
				}
				SendCode(ATCommands::ATIB48_4800BaudRate).Log();
				response = SendCode(OBDCommands::Code_01_ShowCurrentData + OBDPIDs::Code_00_SupportedPIDS).Log().ResponseValue();
				found = response.find("NODATA"s) != std::string::npos;
				if (!found) {
					protocolNotFound = false;
					SendCode(ATCommands::ATDP_DisplayProtocol).Log();
					supportedPINs = response;
					_protocol = protocolNumber;
					continue;
				}
			}
			GetNextProtocol(protocolNumber);

			retry = 0;
		}
		catch (const std::exception& error) {
			if (retry > 5) {
				throw std::runtime_error("Find Protocol for ELM327 failed:"s + error.what());
			}
			++retry;
		}
	}

	SetSupportedPINs(supportedPINs);
}

void ELM327Communicator::SetSupportedPINs(std::string_view supportedPINSAnswer) {
	auto pins_01_20 = supportedPINSAnswer.substr("0100 4100"s.length(), 8);
	auto pinMask = std::stoul(std::string(pins_01_20), nullptr, 16);

	for (int i = 31 ; i >=0; i--) {
		uint32_t mask{ 1 };
		mask = mask << i;
		if ((pinMask & mask) == mask) {
			std::stringstream pinCode;
			pinCode << std::uppercase << std::setfill('0') << std::setw(2) << std::right<< std::hex << (32 - i);
			_supportedPins.push_back(pinCode.str());
		}
	}
}

std::vector<std::pair<std::string, std::string>> ELM327Communicator::ReadCodesValues() {
	std::vector<std::pair<std::string, std::string>> result;
	for (auto& pin : _supportedPins) {
		result.emplace_back(pin, SendCode(OBDCommands::Code_01_ShowCurrentData + pin).ResponseValue());
	}

	return result;
}