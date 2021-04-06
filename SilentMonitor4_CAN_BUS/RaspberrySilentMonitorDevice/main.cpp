#include <string>
#include "BluetoothServer.h"
#include <thread>
#include "SilentMonitorCommunicator.h"


int main(void)
{   std::shared_ptr<BluetoothServer> bsserver = std::make_shared<BluetoothServer>();
    std::thread _bsServerThread([&bsserver]() { bsserver->Run(); });

    SilentMonitorCommunicator communicator(bsserver);

    communicator.Run();
    return 0;
}