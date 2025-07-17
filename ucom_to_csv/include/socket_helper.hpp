#pragma once
#include <string>
#include <vector>

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>
#elif _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#define in_addr_t uint32_t
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
	pollfd _pfd[1];
#elif _WIN32
	SOCKET _socket;
	WSAPOLLFD _pfd[1];
#endif
public:
	Socket(std::string local_ip, int local_port, std::string remote_ip, int remote_port, std::vector<std::string>& errors);
	~Socket();
	int send(const char* buffer, int len, int& error);
	int recv(char* buffer, int max_len, std::string& source_ip, uint32_t &ip_in, int& error);
	bool is_initialised() const { return _initialised; }

	static const std::string get_ip(const in_addr_t *ip);
};