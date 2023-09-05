#include "address.hpp"

#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <net/if.h>

#define _port _address.v4.sin_port

namespace net
{
	Address::Address() :
		_af(invalid)
	{
	}
	Address::Address(in_addr addr, uint16 port) :
		_af(ipv4)
	{
		_port = htons(port);
		_address.v4.sin_len = sizeof(sockaddr_in);
		_address.v4.sin_family = AF_INET;
		_address.v4.sin_addr = addr;
	}
	Address::Address(in6_addr addr, uint16 port) :
		_af(ipv6)
	{
		_port = htons(port);
		_address.v6.sin6_len = sizeof(sockaddr_in6);
		_address.v6.sin6_family = AF_INET6;
		_address.v6.sin6_addr = addr;
	}
	Address::Address(const sockaddr* addr)
	{
		if(addr->sa_family == AF_INET)
		{
			_af = ipv4;
			_address.v4 = *reinterpret_cast<const sockaddr_in*>(addr);
		}
		else if(addr->sa_family == AF_INET6)
		{
			_af = ipv6;
			_address.v6 = *reinterpret_cast<const sockaddr_in6*>(addr);
		}
		else
			_af = invalid;
	}
	Address::Address(const char* ip, uint16 port, addrfamily af) :
		_af(af)
	{
		_address._sa.sa_len = SOCKSIZES[af];
		_address._sa.sa_family = ADDRFAMILIES[af];
		if(ip[0])
		{
			if(af == ipv6 || af == all)
			{
				addrinfo hints = {};
				addrinfo* results;
				int err = getaddrinfo(ip, nullptr, &hints, &results);
				if(results)
				{
					if(results->ai_family == AF_INET6)
					{
						_address.v6 = *reinterpret_cast<sockaddr_in6*>(results->ai_addr);
						_address.v6.sin6_port = htons(port);
					}
					freeaddrinfo(results);
				}
				else
				{
					printf("%s\n", gai_strerror(err));
					invalidate();
					return;
				}
			}
			else
				if(inet_pton(ADDRFAMILIES[af], ip, (af == ipv4) ? reinterpret_cast<void*>(&_address.v4.sin_addr) : reinterpret_cast<void*>(&_address.v6.sin6_addr)) != 1)
				{
					invalidate();
					perror("inet_pton");
					return;
				}
		}
		else
		{
			if(af == ipv4)
				_address.v4.sin_addr.s_addr = INADDR_ANY;
			else
				_address.v6.sin6_addr = in6addr_any;
		}
		_port = htons(port);

		if(af == ipv6 || af == all)
		{
			_address.v6.sin6_flowinfo = 0;
			_address.v6.sin6_scope_id = 0;
		}

		
	}
	Address::Address(const char* addr, const char* port, addrfamily af, sockettype type) :
		_af(af)
	{
		addrinfo hints;
		addrinfo* results;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = (af == all) ? AF_UNSPEC : ADDRFAMILIES[af];
		hints.ai_socktype = SOCKTYPES[type];

		if((getaddrinfo(addr, port, &hints, &results)) != 0)
		{
			perror("getaddrinfo");
			invalidate();
			return;
		}

		memcpy(&_address, results->ai_addr, results->ai_addrlen);

		freeaddrinfo(results);
	}
}