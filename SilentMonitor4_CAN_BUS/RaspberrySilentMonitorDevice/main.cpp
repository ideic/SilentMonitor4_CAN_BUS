#include "BluetoothServer.h"
#include <thread>
#include "SilentMonitorCommunicator.h"
#include "ConfigurationManager.h"
#include "WIFIServer.h"
#include "GlobalComponentState.h"

int main(void){
    std::shared_ptr configManager = std::make_shared<ConfigurationManager>();
    GlobalComponentState globalState;

    std::shared_ptr<BluetoothServer> bsserver = std::make_shared<BluetoothServer>(configManager);
    std::thread _bsServerThread([&bsserver]() { bsserver->Run(); });
    globalState.AddComponent(bsserver);

    std::shared_ptr<WIFIServer> wifiserver = std::make_shared<WIFIServer>(configManager);
    std::thread _wifiServerThread([&wifiserver]() { wifiserver->Run(); });
    globalState.AddComponent(wifiserver);

    std::shared_ptr<SilentMonitorCommunicator> communicator = std::make_shared<SilentMonitorCommunicator>(bsserver, configManager);
    globalState.AddComponent(communicator);
    communicator->Run();

    if (_bsServerThread.joinable()) _bsServerThread.join();
    if (_wifiServerThread.joinable()) _wifiServerThread.join();

    return 0;
}