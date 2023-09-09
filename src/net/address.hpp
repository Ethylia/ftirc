#pragma once

#include <netinet/in.h>

#include "net.hpp"
#include "common.hpp"

namespace net
{
	struct Address
	{
		Address();

		Address(const Address& other);
		Address& operator=(const Address& other);

		Address(in_addr addr, uint16 port);
		Address(const sockaddr* addr);
		Address(const char* ip, uint16 port); // For numeric addresses
		Address(const char* addr, const char* port, sockettype type = none); // For dns lookup

		operator sockaddr*() { return &_address._sa; }
		operator const sockaddr*() const { return &_address._sa; }
		operator in_addr() const { return _address.v4.sin_addr; }
		operator const in_addr*() const { return &_address.v4.sin_addr; }

		const void* addr_data() const { return reinterpret_cast<const void*>(&_address.v4.sin_addr); }

		bool valid() const { return _af != invalid; }

		inline addrfamily af() const { return _af; }
		inline socklen_t size() const { return SOCKSIZES[_af]; }
		inline in_port_t port() const { return ntohs(_address.v4.sin_port); }

		inline void invalidate() {_af = invalid;}

	private:
		addrfamily _af;

		union _sockaddress
		{
			sockaddr_in  v4;
			sockaddr _sa;
		} _address;
	};
}
