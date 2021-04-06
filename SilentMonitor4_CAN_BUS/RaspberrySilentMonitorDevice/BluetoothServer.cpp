#include "BluetoothServer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <vector>
#include "Logger.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

using namespace std::string_literals;

struct BluetoothServer::SocketInfo {
    int _socketId{ -1 };
    int _client{ -1 };
    bdaddr_t _address{};
};

BluetoothServer::BluetoothServer()
{
    _socketInfo = std::make_shared<SocketInfo>();

    const int dev_id = hci_get_route(nullptr);
    const int sock = hci_open_dev(dev_id);
    if (dev_id < 0 || sock < 0) {
        throw std::runtime_error("Cannot open bluetooth socket");
    }

    hci_read_bd_addr(sock, &_socketInfo->_address, 1000);
    char addr[19] = { 0 };
    ba2str(&_socketInfo->_address, addr);
    Logger::Info("Bluetooth address: "s + addr);
}

BluetoothServer::~BluetoothServer()
{
    if (_socketInfo->_client >=0)
        close(_socketInfo->_client);
    if (_socketInfo->_socketId >= 0)
        close(_socketInfo->_socketId);
}

void BluetoothServer::Stop()
{
    _stopped = true;
}


void BluetoothServer::Run()
{
    //RegisterService();
    struct sockaddr_rc loc_addr = { 0 };
    // allocate socket
    _socketInfo->_socketId = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (_socketInfo->_socketId < 0) {
        throw std::runtime_error("Bluetooth socket Init failed errno:"s + std::to_string(errno) + " Reason: "s + std::string(strerror(errno)));
    }


    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    //auto any = (bdaddr_t){ {0x00, 0x1A, 0x7D, 0xDA, 0x71, 0x15} };
    loc_addr.rc_bdaddr = _socketInfo->_address;
    loc_addr.rc_channel = static_cast<uint8_t>(1);
    const auto result = bind(_socketInfo->_socketId, (struct sockaddr*)&loc_addr, sizeof(loc_addr));
    if (result < 0) {
        throw std::runtime_error("Bluetooth socket Binde failed errno:"s + std::to_string(errno) + " Reason: "s + std::string(strerror(errno)));
    }

    // put socket into listening mode
    listen(_socketInfo->_socketId, 1);
    Connect();
    
    while (!_stopped) {
        std::vector<uint8_t> buf(1024, 0);
        // read data from the client
        const auto bytes_read = read(_socketInfo->_client, buf.data(), buf.size());
        if (bytes_read > 0) {
            _receivedCommands.push(std::string(begin(buf), end(buf)));
        }
        else if (bytes_read == 0) {
            std::this_thread::sleep_for(100ms);
        }
        else {
            Connect();
        }
    }
}
void BluetoothServer::SendCommand(const std::string & command) const {
	const auto result = write(_socketInfo->_client, command.c_str(), command.size());
    if (result < 0) {
        throw std::runtime_error("Send Error errno:"s + std::to_string(errno) + " Reason: "s + std::string(strerror(errno)));
    }
}

void BluetoothServer::Connect(){
    struct sockaddr_rc rem_addr = { 0 };
    socklen_t opt = sizeof(rem_addr);

    // accept one connection
    _socketInfo->_client = accept(_socketInfo->_socketId, (struct sockaddr*)&rem_addr, &opt);

    std::vector<uint8_t> buf(1024, 0);
 
    ba2str(&rem_addr.rc_bdaddr, (char*)buf.data());

    Logger::Info("Accepted connection from "s + std::string((char*)buf.data()) + "\n"s);

    buf.clear();
    buf.resize(1024);
}