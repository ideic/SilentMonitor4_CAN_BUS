#include "CANBUSCommunicator.h"
#include "DateTimeProvider.h"

CANBUSCommunicator::CANBUSCommunicator(std::shared_ptr<ConfigurationManager> configManager) : _configuraionManager(std::move(configManager)){
	_tcpClient = std::make_shared<TCPClient>(_configuraionManager->GetWifiSetting().Host, _configuraionManager->GetWifiSetting().Port);
	_tcpClient->Connect();
	_elm327 = std::make_unique<ELM327Communicator>(_tcpClient);
	_elm327->Connect();
	_dataLogger = RollingFileLogger(_configuraionManager->GetWorkingDir() + DateTimeProvider::DateTme()->UtcNow().DateString());
}

void CANBUSCommunicator::Fetch()
{
	try {
		_dataLogger.Info("Live Codes");
		for (auto& [code, value] : _elm327->ReadLiveCodeValues()) {
			_dataLogger.Info(code + " "s + value);
		}
		_dataLogger.Info("VOLT");
		auto& [voltage, value] = _elm327->ReadVoltageCodeValue();
		_dataLogger.Info(voltage + " "s + value);

		_dataLogger.Info("Error Codes");
		for (auto& [errorcode, errorvalue] : _elm327->ReadErrorCodeValues()) {
			_dataLogger.Error(errorcode + " "s + errorvalue);
		}
	}
	catch (const std::exception& e) {
		Logger::Error("Exception occured:"s + e.what());
	}
}
