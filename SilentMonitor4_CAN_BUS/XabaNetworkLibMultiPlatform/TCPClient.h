#pragma once
#include <string>
#include <thread>
#include <memory>
#include <string_view>
#include <vector>
namespace Xaba {
	class TCPClient
	{
		struct SocketInfo;
		std::string _host;
		std::string _port;

		std::shared_ptr<SocketInfo> _socketInfo;

	public:
		TCPClient(std::string host, std::string port);
		~TCPClient();
		void Connect();

		virtual std::vector<uint8_t> ReceiveData();
		virtual void SendData(std::string_view message);
	};
}
