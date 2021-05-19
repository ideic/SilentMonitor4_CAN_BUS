#pragma once
#include <string>
#include <thread>
#include <memory>
#include <BlockingQueue.h>
class TCPClient
{
	struct SocketInfo;
	std::string _host;
	std::string _port;
	std::thread _workerThread;
	
	std::atomic_bool _stopped;
	std::shared_ptr<SocketInfo> _socketInfo;
	BlockingQueue<std::vector<uint8_t>> _queue;

	void Core();
public:
	TCPClient(std::string host, std::string port);
	~TCPClient();
	void Start();
	void Stop();

	std::vector<uint8_t> ReceiveData();
};

