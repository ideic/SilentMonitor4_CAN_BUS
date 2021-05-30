#include "pch.h"
#include <TCPClient.h>
#include "ELM327Communicator.h"
#include <string_view>
#include <algorithm>
#pragma comment( lib, "Ws2_32.lib" )
using namespace Xaba;
class TCPClientMock : public TCPClient {
private:
	std::map<std::string, std::string> _atAnswers;
	std::vector<std::tuple<std::string, std::string, std::string>> _obdAnswers;
	std::string _sentATData;
	std::string _sentOBDData;
	std::string _lastCommandType;

public:
	TCPClientMock(std::string host, std::string port) : TCPClient(std::move(host), std::move(port)) {};

	void SetATAnswers(std::map<std::string, std::string> answer) {
		_atAnswers = std::move(answer);
	}

	void SetOBDAnswers(std::vector<std::tuple<std::string, std::string, std::string>> answer) {
		_obdAnswers = std::move(answer);
	}

	std::vector<uint8_t> ReceiveData() override {
		if (_lastCommandType == "AT"s) {
			return std::vector<uint8_t>(begin(_atAnswers[_sentATData]), end(_atAnswers[_sentATData]));
		}

		auto obdAnswer = std::find_if(begin(_obdAnswers), end(_obdAnswers), [this](auto& triple) {
			std::string atcode, obdcode, answer;
			std::tie(atcode, obdcode, answer) = triple;
			return atcode == _sentATData && _sentOBDData == obdcode;
		});

		if (obdAnswer != end(_obdAnswers)) {
			auto [atcode, obdcode, answer] = *obdAnswer;
			return std::vector<uint8_t>(begin(answer), end(answer));
		}
		return {};
	}
	void SendData(std::string_view message) override {
		if (message._Starts_with("AT")) {
			_sentATData = message;
			_lastCommandType = "AT"s;
		}
		else {
			_sentOBDData = message;
			_lastCommandType = "OBD"s;
		}
	}
};
TEST(ELM327CommunicatorTest, ConnectFindsISO99412) {
	std::shared_ptr<TCPClientMock> tcpMock = std::make_shared<TCPClientMock>(""s,""s);
	tcpMock->SetATAnswers(
		{
			{"ATI\r"s, "ATI\rELM327 v1.5\r\r>"s},
			{"ATD\r"s, "ATD\rOK\r\r>"s},
			{"ATZ\r"s, "ATI\rELM327 v1.5\r\r>"s},
			{"ATE1\r"s, "ATE1\rOK\r\r>"s},
			{"ATL0\r"s, "ATL0\rOK\r\r>"s},
			{"ATSP0\r"s, "ATSP0\rOK\r\r>"s},
			{"ATSP1\r"s, "ATSP1\rOK\r\r>"s},
			{"ATSP2\r"s, "ATSP2\rOK\r\r>"s},
			{"ATSP3\r"s, "ATSP3\rOK\r\r>"s},
			{"ATSP4\r"s, "ATSP4\rOK\r\r>"s},
			{"ATSP5\r"s, "ATSP5\rOK\r\r>"s},
			{"ATSP6\r"s, "ATSP6\rOK\r\r>"s},
			{"ATSP7\r"s, "ATSP7\rOK\r\r>"s},
			{"ATSP8\r"s, "ATSP8\rOK\r\r>"s},
			{"ATSP9\r"s, "ATSP9\rOK\r\r>"s},
			{"ATSPA\r"s, "ATSPA\rOK\r\r>"s},
			{"ATIB96\r"s, "ATIB96\rOK\r\r>"s},
			{"ATIB48\r"s, "ATIB48\rOK\r\r>"s},

		});
	tcpMock->SetOBDAnswers(
		{
			{"ATSP0\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP1\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP2\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP3\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATIB96\r"s, "0100\r"s, "0100\r41 00 00 00 00 00\r\r>"s},

		});

	ELM327Communicator device(tcpMock);

	EXPECT_NO_THROW( device.Connect());
}

TEST(ELM327CommunicatorTest, ReadLiveCodes) {
	std::shared_ptr<TCPClientMock> tcpMock = std::make_shared<TCPClientMock>(""s, ""s);
	tcpMock->SetATAnswers(
		{
			{"ATI\r"s, "ATI\rELM327 v1.5\r\r>"s},
			{"ATD\r"s, "ATD\rOK\r\r>"s},
			{"ATZ\r"s, "ATI\rELM327 v1.5\r\r>"s},
			{"ATE1\r"s, "ATE1\rOK\r\r>"s},
			{"ATL0\r"s, "ATL0\rOK\r\r>"s},
			{"ATSP0\r"s, "ATSP0\rOK\r\r>"s},
			{"ATSP1\r"s, "ATSP1\rOK\r\r>"s},
			{"ATSP2\r"s, "ATSP2\rOK\r\r>"s},
			{"ATSP3\r"s, "ATSP3\rOK\r\r>"s},
			{"ATSP4\r"s, "ATSP4\rOK\r\r>"s},
			{"ATSP5\r"s, "ATSP5\rOK\r\r>"s},
			{"ATSP6\r"s, "ATSP6\rOK\r\r>"s},
			{"ATSP7\r"s, "ATSP7\rOK\r\r>"s},
			{"ATSP8\r"s, "ATSP8\rOK\r\r>"s},
			{"ATSP9\r"s, "ATSP9\rOK\r\r>"s},
			{"ATSPA\r"s, "ATSPA\rOK\r\r>"s},
			{"ATIB96\r"s, "ATIB96\rOK\r\r>"s},
			{"ATIB48\r"s, "ATIB48\rOK\r\r>"s},
			{"ATDP\r"s, "ATDP\rISO9941_2\r\r>"s},
		});
	tcpMock->SetOBDAnswers(
		{
			{"ATSP0\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP1\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP2\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP3\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATIB96\r"s, "0100\r"s, "0100\r41 00 98 3B 80 11\r\r>"s},
			{"ATDP\r"s, "0101\r"s, "0101\r41 01 01 02 03 04\r\r>"s},
			{"ATDP\r"s, "0104\r"s, "0104\r41 04 10\r\r>"s},
			{"ATDP\r"s, "0105\r"s, "0105\r41 05 20\r\r>"s},
			{"ATDP\r"s, "010B\r"s, "010B\r41 0B 10\r\r>"s},
			{"ATDP\r"s, "010C\r"s, "010B\r41 0C 0E 18\r\r>"s},
			{"ATDP\r"s, "010D\r"s, "010D\r41 0D 0E\r\r>"s},
			{"ATDP\r"s, "0110\r"s, "0110\r41 10 0F AA\r\r>"s},
			{"ATDP\r"s, "010F\r"s, "010F\r41 0F 20\r\r>"s},
			{"ATDP\r"s, "0111\r"s, "0111\r41 11 30\r\r>"s},
			{"ATDP\r"s, "011C\r"s, "011C\r41 1C 03\r\r>"s},
			{"ATDP\r"s, "0120\r"s, "0120\r41 20 80 00 00 00\r\r>"s},
			{"ATDP\r"s, "0121\r"s, "0121\r41 21 01 01\r\r>"s},
		});

	ELM327Communicator device(tcpMock);

	EXPECT_NO_THROW(device.Connect());
	auto codeValues= device.ReadLiveCodeValues();

	EXPECT_EQ(codeValues.size(), 12);

	EXPECT_EQ(codeValues[0].first, "01"s); 
	EXPECT_EQ(codeValues[0].second, "01 02 03 04"s );

	EXPECT_EQ(codeValues[1].first, "04"s);
	EXPECT_EQ(codeValues[1].second, "10"s);

	EXPECT_EQ(codeValues[2].first, "05"s);
	EXPECT_EQ(codeValues[2].second, "20"s);

	EXPECT_EQ(codeValues[3].first, "0B"s);
	EXPECT_EQ(codeValues[3].second, "10"s);

	EXPECT_EQ(codeValues[4].first, "0C"s);
	EXPECT_EQ(codeValues[4].second, "0E 18"s);

	EXPECT_EQ(codeValues[5].first, "0D"s);
	EXPECT_EQ(codeValues[5].second, "0E"s);

	EXPECT_EQ(codeValues[6].first, "0F"s);
	EXPECT_EQ(codeValues[6].second, "20"s);

	EXPECT_EQ(codeValues[7].first, "10"s);
	EXPECT_EQ(codeValues[7].second, "0F AA"s);

	EXPECT_EQ(codeValues[8].first, "11"s);
	EXPECT_EQ(codeValues[8].second, "30"s);

	EXPECT_EQ(codeValues[9].first, "1C"s);
	EXPECT_EQ(codeValues[9].second, "03"s);

	EXPECT_EQ(codeValues[10].first, "20"s);
	EXPECT_EQ(codeValues[10].second, "80 00 00 00"s);

	EXPECT_EQ(codeValues[11].first, "21"s);
	EXPECT_EQ(codeValues[11].second, "01 01"s);
}

TEST(ELM327CommunicatorTest, ReadErrorCodes) {
	std::shared_ptr<TCPClientMock> tcpMock = std::make_shared<TCPClientMock>(""s, ""s);
	tcpMock->SetATAnswers(
		{
			{"ATI\r"s, "ATI\rELM327 v1.5\r\r>"s},
			{"ATD\r"s, "ATD\rOK\r\r>"s},
			{"ATZ\r"s, "ATI\rELM327 v1.5\r\r>"s},
			{"ATE1\r"s, "ATE1\rOK\r\r>"s},
			{"ATL0\r"s, "ATL0\rOK\r\r>"s},
			{"ATSP0\r"s, "ATSP0\rOK\r\r>"s},
			{"ATSP1\r"s, "ATSP1\rOK\r\r>"s},
			{"ATSP2\r"s, "ATSP2\rOK\r\r>"s},
			{"ATSP3\r"s, "ATSP3\rOK\r\r>"s},
			{"ATSP4\r"s, "ATSP4\rOK\r\r>"s},
			{"ATSP5\r"s, "ATSP5\rOK\r\r>"s},
			{"ATSP6\r"s, "ATSP6\rOK\r\r>"s},
			{"ATSP7\r"s, "ATSP7\rOK\r\r>"s},
			{"ATSP8\r"s, "ATSP8\rOK\r\r>"s},
			{"ATSP9\r"s, "ATSP9\rOK\r\r>"s},
			{"ATSPA\r"s, "ATSPA\rOK\r\r>"s},
			{"ATIB96\r"s, "ATIB96\rOK\r\r>"s},
			{"ATIB48\r"s, "ATIB48\rOK\r\r>"s},
			{"ATDP\r"s, "ATDP\rISO9941_2\r\r>"s},
		});
	tcpMock->SetOBDAnswers(
		{
			{"ATSP0\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP1\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP2\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATSP3\r"s, "0100\r"s, "NO DATA\r\r>"s},
			{"ATIB96\r"s, "0100\r"s, "0100\r41 00 98 3B 80 11\r\r>"s},
			{"ATDP\r"s, "03\r"s, "03\r43 02 30 02 43 00 00 \r\r>"s},
			{"ATDP\r"s, "0120\r"s, "0120\r41 20 80 00 00 00\r\r>"s},
		});

	ELM327Communicator device(tcpMock);

	EXPECT_NO_THROW(device.Connect());
	auto codeValues = device.ReadErrorCodeValues();

	EXPECT_EQ(codeValues.size(), 2);

	EXPECT_EQ(codeValues[0].first, "03"s);
	EXPECT_EQ(codeValues[0].second, "02 30"s);

	EXPECT_EQ(codeValues[1].first, "03"s);
	EXPECT_EQ(codeValues[1].second, "02 43"s);
}