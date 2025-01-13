#pragma once
#include <string>

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
	sockaddr_in _dest;
	bool _initialised;
#ifdef __linux__
	int _socket;
#elif _WIN32
	SOCKET _socket;
#endif
public:
	Socket(const std::string &local_ip, std::string dest_ip, int port);
	~Socket();
	int send(std::string buffer);
	int recv(uint8_t* buffer, int max_len);
	bool is_initialised() { return _initialised; }
};