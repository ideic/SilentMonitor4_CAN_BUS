#include "BluetoothServer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <vector>
#include "Logger.h"

using namespace std::string_literals;

struct BluetoothServer::SocketInfo {
    int _socketId{ -1 };
    int _client{ -1 };
    bdaddr_t _address{};
};

BluetoothServer::BluetoothServer(std::string address)
{
	_socketInfo = std::make_shared<SocketInfo>();
    str2ba(address.c_str(), &_socketInfo->_address);
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
    RegisterService();
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
    loc_addr.rc_channel = (uint8_t)1;
    auto result = bind(_socketInfo->_socketId, (struct sockaddr*)&loc_addr, sizeof(loc_addr));
    if (result < 0) {
        throw std::runtime_error("Bluetooth socket Binde failed errno:"s + std::to_string(errno) + " Reason: "s + std::string(strerror(errno)));
    }

    // put socket into listening mode
    listen(_socketInfo->_socketId, 1);

    while (!_stopped) {
        struct sockaddr_rc rem_addr = { 0 };
        socklen_t opt = sizeof(rem_addr);

        // accept one connection
        _socketInfo->_client = accept(_socketInfo->_socketId, (struct sockaddr*)&rem_addr, &opt);

        std::vector<uint8_t> buf(1024, 0);
        int bytes_read;

        ba2str(&rem_addr.rc_bdaddr, (char*)buf.data());

        Logger::Info("Accepted connection from "s + std::string((char*)buf.data()) + "\n"s);

        buf.clear();

        // read data from the client
        bytes_read = read(_socketInfo->_client, buf.data(), buf.size());
        if (bytes_read > 0) {
            _receivedCommands.push(std::string(begin(buf), end(buf)));
        }
    }
}
void BluetoothServer::SendCommand(const std::string & command){
    auto result = write(_socketInfo->_client, command.c_str(), command.size());
    if (result < 0) {
        throw std::runtime_error("Send Error errno:"s + std::to_string(errno) + " Reason: "s + std::string(strerror(errno)));
    }
}

void BluetoothServer::RegisterService() {

    sdp_record_t* record = sdp_record_alloc();

    // set the general service ID
    uuid_t svc_uuid;
    uint32_t service_uuid_int[] = { 0, 0, 0, 0xABCD };
    //sdp_uuid128_create(&svc_uuid, &service_uuid_int);
    sdp_uuid16_create(&svc_uuid, 0x1101);
    sdp_set_service_id(record, svc_uuid);

    /* set the service class */
    uuid_t svc_class_uuid;
    sdp_uuid16_create(&svc_class_uuid, SERIAL_PORT_SVCLASS_ID);
    sdp_list_t* svc_class_list = sdp_list_append(0, &svc_class_uuid);
    sdp_set_service_classes(record, svc_class_list);

    /* set the Bluetooth profile information */
    sdp_profile_desc_t profile;
    sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
    profile.version = 0x0100;
    sdp_list_t *profile_list = sdp_list_append(0, &profile);
    sdp_set_profile_descs(record, profile_list);

    // make the service record publicly browsable
    uuid_t root_uuid;
    sdp_list_t* root_list{};
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups(record, root_list);

    //Record state change it any kind of modification
    sdp_set_record_state(record, 3);

    //Service availablity
    sdp_set_service_avail(record, 0xFF);


    sdp_list_t* proto_list{};

    // set l2cap information
    uuid_t l2cap_uuid;
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    sdp_list_t* l2cap_list = sdp_list_append(0, &l2cap_uuid);
    proto_list = sdp_list_append(0, l2cap_list);

    // set rfcomm information
    uuid_t  rfcomm_uuid;
    sdp_list_t* rfcomm_list{};
    sdp_data_t* channel{};

    uint8_t rfcomm_channel = 1;
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
    sdp_list_append(rfcomm_list, channel);
    //proto_list = sdp_list_append(0, rfcomm_list);
    sdp_list_append(proto_list, rfcomm_list);

    // attach protocol information to service record
    sdp_list_t* access_proto_list{};
    access_proto_list = sdp_list_append(0, proto_list);
    sdp_set_access_protos(record, access_proto_list);

    // set the name, provider, and description
    std::string serviceName = "CAN-BUS Sniffer Device";
    std::string serviceDesc = "CAN-BUS sniffer, log all data from CAN-BUS";
    std::string serviceProv = "CAN-BUS Sniffer";
    sdp_set_info_attr(record, serviceName.c_str(), serviceProv.c_str(), serviceDesc.c_str());

    // connect to the local SDP server, register the service record, and 
    bdaddr_t any = { };
    bdaddr_t local{ {0, 0, 0, 0xff, 0xff, 0xff} };
    auto session = sdp_connect(&any, &local, SDP_RETRY_IF_BUSY);
    if (!session) {
        throw std::runtime_error("Bluetoot sdp connect failed"s + std::string(strerror(errno)));
    }
    auto err = sdp_record_register(session, record, SDP_RECORD_PERSIST);
    if (err < 0) {
        throw std::runtime_error("Bluetoot sdp_record_register failed:"s + std::string(strerror(errno)));
    }

     // cleanup
    sdp_data_free(channel);
    //sdp_list_free(l2cap_list, 0);
    sdp_list_free(rfcomm_list, 0);
    sdp_list_free(root_list, 0);
    sdp_list_free(access_proto_list, 0);
}
 

