#pragma once

#include "net/socket.hpp"

class Client
{
public:
	Client();
	~Client();

	operator const int&() const { return _socket; }

	bool accept(const net::Socket& s) { return _socket.accept(s); }

private:
	net::Socket _socket;
};
