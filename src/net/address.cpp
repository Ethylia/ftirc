#include "address.hpp"

#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <net/if.h>

#define _port _address.v4.sin_port

namespace net
{
	Address::Address() :
		_af(invalid), _address()
	{
		_port = 0;
	}
	Address::Address(const Address& other) :
		_af(other._af)
	{
		memcpy(&_address, &other._address, SOCKSIZES[_af]);
	}
	Address& Address::operator=(const Address& other)
	{
		_af = other._af;
		memcpy(&_address, &other._address, SOCKSIZES[_af]);
		return *this;
	}
	Address::Address(in_addr addr, uint16 port) :
		_af(ipv4)
	{
		_port = htons(port);
		// _address.v4.sin_len = sizeof(sockaddr_in);
		_address.v4.sin_family = AF_INET;
		_address.v4.sin_addr = addr;
	}
	Address::Address(const sockaddr* addr)
	{
		if(addr->sa_family == AF_INET)
		{
			_af = ipv4;
			_address.v4 = *reinterpret_cast<const sockaddr_in*>(addr);
		}
		else
			_af = invalid;
	}
	Address::Address(const char* ip, uint16 port) :
		_af(ipv4)
	{
		// _address._sa.sa_len = SOCKSIZES[ipv4];
		_address._sa.sa_family = ADDRFAMILIES[ipv4];
		if(ip[0])
		{
			_address.v4.sin_addr.s_addr = inet_addr(ip);
			if(_address.v4.sin_addr.s_addr == INADDR_NONE)
			{
				invalidate();
				std::cerr << "Failed to resolve address: " << ip << std::endl;
				return;
			}
		}
		else
		{
			_address.v4.sin_addr.s_addr = INADDR_ANY;
		}
		_port = htons(port);
	}
	Address::Address(const char* addr, const char* port, sockettype type) :
		_af(ipv4)
	{
		addrinfo hints;
		addrinfo* results;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = ADDRFAMILIES[ipv4];
		hints.ai_socktype = SOCKTYPES[type];

		if((getaddrinfo(addr, port, &hints, &results)) != 0)
		{
			invalidate();
			std::cerr << "Failed to resolve address: " << addr << ":" << port << std::endl;
			return;
		}
		memcpy(&_address, results->ai_addr, results->ai_addrlen);
		freeaddrinfo(results);
	}
}
