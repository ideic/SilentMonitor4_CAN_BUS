#include "pch.h"
#include "TCPClient.h"
#include "ELM327Communicator.h"
#include <string_view>
#include <algorithm>
#pragma comment( lib, "Ws2_32.lib" )
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
			{"ATSP0\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATSP1\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATSP2\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATSP3\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATIB96\r"s, "0100\r"s, "0100\r4100983B8011\r\r>"s},

		});

	ELM327Communicator device(tcpMock);

	EXPECT_NO_THROW( device.Connect());
}

TEST(ELM327CommunicatorTest, ReadCodesThat0100GivesBack) {
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

		});
	tcpMock->SetOBDAnswers(
		{
			{"ATSP0\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATSP1\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATSP2\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATSP3\r"s, "0100\r"s, "NODATA\r\r>"s},
			{"ATIB96\r"s, "0100\r"s, "0100\r4100983B8011\r\r>"s},

		});

	ELM327Communicator device(tcpMock);

	EXPECT_NO_THROW(device.Connect());
	auto codeValues= device.ReadCodesValues();

	EXPECT_EQ(codeValues.size(), 11);

	EXPECT_EQ(codeValues[0].first, "01"s); 
	EXPECT_EQ(codeValues[0].second, ""s );
	és így tovább;



}

