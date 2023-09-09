#include "socket.hpp"

#include <unistd.h>
//#include <netdb.h>
//#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <iostream>

namespace net
{
	Socket::Socket() : _sockfd(-1), _bound(false), _connected(false)
	{
	}
	Socket::Socket(sockettype type) : _bound(false), _connected(false)
	{
		create(type);
	}
	Socket::~Socket()
	{
		if(valid())
			close();
	}

	bool Socket::create(sockettype type)
	{
		_sockfd = socket(ADDRFAMILIES[ipv4], SOCKTYPES[type], ((type == tcp) ? IPPROTO_TCP : IPPROTO_UDP));
		if(_sockfd == -1)
			return perror("socket"), false;
		int e = fcntl(_sockfd, F_SETFL, O_NONBLOCK);
		if(e == -1)
		{
			std::cerr << "Failed to set socket to non-blocking" << std::endl;
			return false;
		}
		return _sockfd != -1;
	}

	bool Socket::bind(in_addr addr, uint16 port)
	{
		if(_bound)
			return false;
		sockaddr_in saddr = {};
		saddr.sin_port = htons(port);
		saddr.sin_addr = addr;
		saddr.sin_family = AF_INET;
		const int opt = 1;
		setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
		if(::bind(_sockfd, reinterpret_cast<sockaddr*>(&saddr), sizeof(sockaddr_in)) == 0)
			_bound = true;
		else
			perror("bind");
		return _bound;
	}
	bool Socket::bind(const Address& a)
	{
		if(_bound)
			return false;
		const int opt = 1;
		setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
		if(::bind(_sockfd, a, SOCKSIZES[a.af()]) == 0)
			_bound = true;
		else
			perror("bind");
		return _bound;
	}

	bool Socket::connect(const Address& addr)
	{
		if(::connect(_sockfd, addr, SOCKSIZES[addr.af()]) == 0)
		{
			_connected = true;
			_bound = true;
			return true;
		}
		perror("connect");
		return false;
	}

	bool Socket::send(const char* data, uint32 size) const
	{
		if(!_connected || !valid())
			return false;
		return ::send(_sockfd, data, size, 0) != -1;
	}
	void Socket::sendto(const byte* data, uint32 size, const Address& addr)
	{
		::sendto(_sockfd, data, size, 0, addr, addr.size());
	}

	ssize_t Socket::receive(void* data, uint32 size)
	{
		ssize_t r = recv(_sockfd, data, size, 0);
		return r;
	}

	bool Socket::listen(uint32 backlog)
	{
		if(!_bound || !valid())
			return false;
		if(::listen(_sockfd, backlog) == 0)
			return true;
		return false;
	}

	bool Socket::accept(const Socket& as)
	{
		if(!as.bound() || !as.valid())
			return false;
		sockaddr_storage saddrs = {};
		sockaddr& saddr = reinterpret_cast<sockaddr&>(saddrs);
		socklen_t len = sizeof(sockaddr_storage);
		int fd = ::accept(as, &saddr, &len);
		if(fd < 1 || len != sizeof(sockaddr_in)) // we don't support ipv6
			return false;
		_connected = true;
		_bound = true;
		_sockfd = fd;
		return true;
	}

	void Socket::close()
	{
		::close(_sockfd);
		_sockfd = -1;
		_bound = false;
		_connected = false;
	}
}
