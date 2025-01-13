#include "socket_helper.hpp"
#ifdef __linux__

#elif _WIN32

#endif


Socket::Socket(const std::string &local_ip, std::string dest_ip, int port) :
	_dest{0,0,0,0},
	_local{0,0,0,0},
	_initialised{false},
	_socket{0}
{
#ifdef _WIN32
	// Initialise WinSock
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;
#endif
	_local.sin_family = AF_INET;
	_local.sin_port = htons(port);
	inet_pton(AF_INET, local_ip.c_str(), &_local.sin_addr.s_addr);

	_dest.sin_family = AF_INET;
	_dest.sin_port = htons(port);
	inet_pton(AF_INET, dest_ip.c_str(), &_dest.sin_addr.s_addr);

	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (bind(_socket, (sockaddr*)&_local, sizeof(_local)) != 0)
		return;

	_initialised = true;
}

Socket::~Socket()
{
#ifdef __linux__
	close(_socket);
#elif _WIN32
	closesocket(_socket);
	WSACleanup();
#endif
}

int Socket::send(std::string buffer)
{
	if (!is_initialised())
		return 0;

	return sendto(_socket, buffer.c_str(), buffer.size(), 0, (sockaddr*)&_dest, sizeof(_dest));
}

int Socket::recv(uint8_t* buffer, int max_len)
{
	if (!is_initialised())
		return 0;

	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	return recvfrom(_socket, (char*)buffer, max_len, 0, (struct sockaddr*)&clientAddr, &addrLen);
}


