#include "BluetoothServer.h"
#include <thread>
#include "SilentMonitorCommunicator.h"
#include "ConfigurationManager.h"

int main(void)
{   std::shared_ptr<BluetoothServer> bsserver = std::make_shared<BluetoothServer>();
    std::thread _bsServerThread([&bsserver]() { bsserver->Run(); });
    std::shared_ptr configManager = std::make_shared<ConfigurationManager>();

    SilentMonitorCommunicator communicator(bsserver, configManager);

    communicator.Run();
    return 0;
}