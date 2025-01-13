#pragma once
#include <string>
#include <vector>

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#elif _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif
/// <summary>
/// Abstracts socket creation to provide a simple interface for sending data
/// </summary>
class Socket {
private:
	sockaddr_in _local;
	sockaddr_in _remote;
	std::string _local_ip;
	std::string _remote_ip;
	int _local_port;
	int _remote_port;
	bool _initialised;
#ifdef __linux__
	int _socket;
#elif _WIN32
	SOCKET _socket;
#endif
public:
	Socket(std::string local_ip, int local_port, std::string remote_ip, int remote_port, std::vector<std::string>& errors);
	~Socket();
	int send(const char* buffer, int len, int& error);
	int recv(char* buffer, int max_len, int& error);
	bool is_initialised() { return _initialised; }
};