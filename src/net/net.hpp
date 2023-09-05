#pragma once

#include <netinet/in.h>

#define LOOPBACK {INADDR_LOOPBACK}
#define IPV6LOOPBACK {IN6ADDR_LOOPBACK_INIT}
#define INITADDR {INADDR_ANY}
#define IPV6INITADDR {IN6ADDR_ANY_INIT}

namespace net
{
	enum addrfamily
	{
		invalid = 0,
		ipv4,
		ipv6,
		all
	};

	const char* const ADDRFAMILYSTRINGS[] = {"Invalid", "IPv4", "IPv6", "All"};

	enum sockettype
	{
		none = 0,
		tcp,
		udp,
		typecount
	};

	const char* const SOCKETTYPESTRINGS[] = {"None", "TCP", "UDP"};

	extern const int ADDRFAMILIES[];
	extern const socklen_t SOCKSIZES[];
	extern const int SOCKTYPES[];
}
