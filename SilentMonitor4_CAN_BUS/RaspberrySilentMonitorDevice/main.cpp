#include "BluetoothServer.h"
#include <thread>
#include "SilentMonitorCommunicator.h"
#include "ConfigurationManager.h"
#include "CANBUSCommunicator.h"
#include <Logger/Logger.h>
#include "ThreadManager.h"
using namespace std::string_literals;
using namespace Xaba;
int main(void){

	try {
        std::shared_ptr<ConfigurationManager> _configManager = std::make_shared<ConfigurationManager>();

        Logger::InitLogger(_configManager->GetLogSetting().LogSinks, Logger::LogContext{ _configManager->GetWorkingDir() + "canbussniffer"s });
	
        ThreadManager<BluetoothServer> bsserverThread (std::make_shared<BluetoothServer>(_configManager));

        ThreadManager<CANBUSCommunicator> canbusCommunicator (std::make_shared<CANBUSCommunicator>(_configManager));

        std::shared_ptr<SilentMonitorCommunicator> communicator = std::make_shared<SilentMonitorCommunicator>(bsserverThread.GetInstance(), _configManager, canbusCommunicator.GetInstance());
        communicator->Run();

        if (_configManager->IsRestartNeeded()) {
            system("sudo reboot");
        }
    }
    catch (const std::exception & error) {
        Logger::Error(error.what());
    }

    return 0;
}
