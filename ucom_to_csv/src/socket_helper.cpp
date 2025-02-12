#include "socket_helper.hpp"
#ifdef __linux__
#include <errno.h>
#define SOCKLEN_T_PTR socklen_t *
#elif _WIN32
#define SOCKLEN_T_PTR int *
#endif

Socket::Socket(std::string local_ip, int local_port, std::string remote_ip, int remote_port, std::vector<std::string>& errors) :
    _local_ip(local_ip),
    _remote_ip(remote_ip),
    _local_port(local_port),
    _remote_port(remote_port)
{
    bool success = false;
#ifdef _WIN32
    // For Windows, set up WSA
    WSAData data;
    int wsa_result = WSAStartup(MAKEWORD(2, 2), &data);
    if (wsa_result != NO_ERROR) {
        errors.push_back(std::string("WSAStartup failed with error ").append(std::to_string(wsa_result)));
        return;
    }

#endif
    _local.sin_family = AF_INET;
    if (local_ip.compare("0.0.0.0") == 0)
        _local.sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, local_ip.c_str(), &_local.sin_addr.s_addr);
    _local.sin_port = htons(local_port);

    _remote.sin_family = AF_INET;
    inet_pton(AF_INET, remote_ip.c_str(), &_remote.sin_addr.s_addr);
    _remote.sin_port = htons(remote_port);

    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    const int reuse = 1;
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
#ifdef __linux__
    setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse));
#endif

    // Bind (for incoming data / server  - associates socket with local IP address / port)
    success = bind(_socket, (sockaddr*)&_local, sizeof(_local)) == 0;

    if (!success)
    {
#ifdef __linux__
        errors.push_back(std::string("Socket bind failed: ").append(errno)));
#elif _WIN32
        errors.push_back(std::string("Socket bind failed: ").append(std::to_string(WSAGetLastError())));
#endif
    }

    // Set up poll
    if (success)
    {
        _pfd[0].fd = _socket;
        _pfd[0].events = POLLIN;
    }
    _initialised = success;
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

int Socket::send(const char* buffer, int len, int& error)
{
    _remote.sin_family = AF_INET;
    inet_pton(AF_INET, _remote_ip.c_str(), &_remote.sin_addr.s_addr);
    _remote.sin_port = htons(_remote_port);

    int result = sendto(_socket, buffer, len, 0, (sockaddr*)&_remote, sizeof(_remote));

    if (result < 0)
#ifdef __linux__
        error = errno;
#elif _WIN32
        error = WSAGetLastError();
#endif
    return result;
}

int Socket::recv(char* buffer, int len, std::string &source_ip, uint32_t &ip_in, int& error)
{
    int addr_len = sizeof(_remote);
    int result = 0;

    // Poll socket, with a timeout of 1000ms
    int count;
#ifdef __linux__
    count = poll(_pfd, 1, 1000);
#elif _WIN32
    count = WSAPoll(_pfd, 1, 1000);
#endif
    if (count == 0)
    {
        // Timed out 
        return 0;
    }
#ifdef __linux__
    else if (count == -1)
    {
        error = errno;
        return -1;
    }
#elif _WIN32
    if (count == SOCKET_ERROR)
    {
        error = WSAGetLastError();
        return -1;
    }
#endif
        
    result = recvfrom(_socket, buffer, len, 0, (sockaddr*)&_remote, (SOCKLEN_T_PTR) &addr_len);
    in_addr_t in_ip = _remote.sin_addr.s_addr;
    
#ifdef __linux__
    if (result < 0)
        error = errno;
#elif _WIN32
    if (result == SOCKET_ERROR)
        error = WSAGetLastError();
#endif

    if (error == 0)
    {
        // Update ip_in
        ip_in = _remote.sin_addr.s_addr;
        // Obtain remote IP address as a std::string
        source_ip = std::move(get_ip(&ip_in));
    }
    return result;
}

// Convert a binary IP address (AF_INET) to a string
const std::string Socket::get_ip(const in_addr_t *ip)
{
    char ip_str[INET_ADDRSTRLEN] = { '\0' };
    inet_ntop(AF_INET, ip, ip_str, INET_ADDRSTRLEN);
    return std::string(ip_str);
}
