#include "net.hpp"

//#include <sys/socket.h>

namespace net
{
	const int ADDRFAMILIES[] =
		{
			0,       // invalid
			AF_INET, // ipv4
			AF_INET6,// ipv6
			AF_INET6 // all
		};

	const socklen_t SOCKSIZES[] =
		{
			0,
			sizeof(sockaddr_in), // ipv4
			sizeof(sockaddr_in6),// ipv6
			sizeof(sockaddr_in6) // all
		};

	const int SOCKTYPES[] =
		{
			0,           // none
			SOCK_STREAM, // tcp
			SOCK_DGRAM   // udp
		};
}
