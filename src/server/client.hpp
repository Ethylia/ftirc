#pragma once

#include "net/sockets.hpp"

class Client
{
public:
	Client();
	~Client();

	bool accept(const net::Socket& s);

private:
	net::Socket _socket;
};
