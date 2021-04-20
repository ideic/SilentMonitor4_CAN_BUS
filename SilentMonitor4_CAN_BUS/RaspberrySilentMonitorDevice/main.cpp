#include "BluetoothServer.h"
#include <thread>
#include "SilentMonitorCommunicator.h"
#include "ConfigurationManager.h"
#include "WIFIServer.h"
#include "Logger.h"
int main(void){
    try {
        std::shared_ptr configManager = std::make_shared<ConfigurationManager>();

        std::shared_ptr<BluetoothServer> bsserver = std::make_shared<BluetoothServer>(configManager);
        std::thread _bsServerThread([&bsserver]() { bsserver->Run(); });

        std::shared_ptr<WIFIServer> wifiserver = std::make_shared<WIFIServer>(configManager);
        std::thread _wifiServerThread([&wifiserver]() { wifiserver->Run(); });

        std::shared_ptr<SilentMonitorCommunicator> communicator = std::make_shared<SilentMonitorCommunicator>(bsserver, configManager);
        communicator->Run();

        if (_bsServerThread.joinable()) _bsServerThread.join();
        if (_wifiServerThread.joinable()) _wifiServerThread.join();

        if (configManager->IsRestartNeeded()) {
            system("sudo reboot");
        }
    }
    catch (const std::exception & error) {
        Logger::Error(error.what());
    }
    return 0;
}