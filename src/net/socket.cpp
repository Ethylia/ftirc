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

namespace net
{
	Socket::Socket()
	{
	}
	Socket::Socket(addrfamily af, sockettype type)
	{
		create(af, type);
	}
	Socket::~Socket()
	{
		if(valid())
			close();
	}

	bool Socket::create(addrfamily af, sockettype type)
	{
		_sockfd = socket(ADDRFAMILIES[af], SOCKTYPES[type], ((type == tcp) ? IPPROTO_TCP : IPPROTO_UDP));
		if(_sockfd == -1)
			return perror("socket"), false;
		// int e = fcntl(_sockfd, F_SETFL, fcntl(_sockfd, F_GETFL, 0) | O_NONBLOCK);
		// if(e == -1)
		// 	return perror("fcntl"), close(), false;
		if(af == all)
			setoption(IPV6_V6ONLY, IPPROTO_IPV6, 0);
		else if(af == ipv6)
			setoption(IPV6_V6ONLY, IPPROTO_IPV6, 1);
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

	void Socket::send(const char* data, uint32 size)
	{
		if(!_connected || !valid())
			return;
		::send(_sockfd, data, size, 0);
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

	bool Socket::accept(Socket& s)
	{
		if(!_bound || !valid())
			return false;
		sockaddr_storage saddrs = {};
		sockaddr& saddr = reinterpret_cast<sockaddr&>(saddrs);
		socklen_t len = sizeof(sockaddr_storage);
		int fd = ::accept(_sockfd, &saddr, &len);
		if(fd != -1)
		{
			s._sockfd = fd;
			s._connected = true;
			s._listening = false;
			s._af = reinterpret_cast<sockaddr&>(saddr).sa_family == AF_INET ? ipv4 : ipv6;
			s._type = tcp;
			if(saddr.sa_family == AF_INET)
				s._peeraddr = Address(reinterpret_cast<sockaddr_in&>(saddr).sin_addr, ntohs(reinterpret_cast<sockaddr_in&>(saddr).sin_port));
			else if(saddr.sa_family == AF_INET6)
				s._peeraddr = Address(reinterpret_cast<sockaddr_in6&>(saddr).sin6_addr, ntohs(reinterpret_cast<sockaddr_in6&>(saddr).sin6_port));
			len = sizeof(sockaddr_storage);
			getsockname(fd, reinterpret_cast<sockaddr*>(&saddrs), &len);
			s._bound = true;
			s._addr = Address(reinterpret_cast<sockaddr*>(&saddrs));
			_accepting = false;
			return true;
		}
		_accepting = false;
		return false;
	}

	void Socket::close()
	{
		::close(_sockfd);
		_sockfd = -1;
		_bound = false;
		_connected = false;
		_listening = false;
	}
}
