#include "TCPClient.h"
#include <functional>
#include <vector>
#ifdef WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#define MY_INVALID_SOCKET INVALID_SOCKET
#define MY_SOCKET_ERROR SOCKET_ERROR
#define MY_GET_LAST_ERROR WSAGetLastError()
#define MY_GET_ERROR_MESSAGE GetErrorMessage
#define INIT_SOCKET InitSocketCommunicaiton()
#endif
#ifdef LINUX
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define MY_INVALID_SOCKET -1
#define MY_SOCKET_ERROR SOCKET_ERROR  -1
#define MY_GET_LAST_ERROR errno
#define MY_GET_ERROR_MESSAGE strerror
#define INIT_SOCKET ;
#endif // LINUX
#include <iostream>
using namespace std::string_literals;

std::string GetErrorMessage(int systemtErrorCode)
{
    char* message;
    auto msgLength = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
        NULL,
        systemtErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
        NULL
    );

    return std::string(message, msgLength);
}

void InitSocketCommunicaiton() {
    WSADATA wsaData = { 0 };
    auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("Error at WSA Init"s + std::to_string(iResult));
    }
}

struct TCPClient::SocketInfo {
    SOCKET socketId{ MY_INVALID_SOCKET };
};

TCPClient::TCPClient(std::string host, std::string port): _host(host), _port(port){
    _socketInfo = std::make_shared<SocketInfo>();
}

TCPClient::~TCPClient(){
     closesocket(_socketInfo->socketId);
}


std::vector<uint8_t> TCPClient::ReceiveData()
{
    std::vector<uint8_t> buff(4096);
    auto readByte = recv(_socketInfo->socketId, (char*)buff.data(), buff.size(), 0);
    buff.resize(readByte);
    return buff;
}

void TCPClient::SendData(std::string_view message){
    auto res = send(_socketInfo->socketId, message.data(), message.length(), 0);
}

void TCPClient::Connect() {
    INIT_SOCKET;
    struct sockaddr_in servaddr {};

    // socket create and varification
    _socketInfo->socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socketInfo->socketId == MY_INVALID_SOCKET) {
        auto error = MY_GET_LAST_ERROR;
        throw std::runtime_error("TCP Socket Init failed:"s + std::to_string(error) + " Reason: "s + MY_GET_ERROR_MESSAGE(error));
    }

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    inet_pton(servaddr.sin_family, _host.c_str(), &servaddr.sin_addr.s_addr);
    // servaddr.sin_addr.s_addr = ( inet_addr(_host.c_str());
    servaddr.sin_port = htons(stoi(_port));

    // connect the client socket to server socket
    if (connect(_socketInfo->socketId, (sockaddr*)&servaddr, sizeof(servaddr)) == MY_SOCKET_ERROR) {
        auto error = MY_GET_LAST_ERROR;
        throw std::runtime_error("TCP Connect failed:"s + std::to_string(error) + " Reason: "s + MY_GET_ERROR_MESSAGE(error));
    }
}

