#include "client.hpp"

#include "server.hpp"

#include <iostream>

Client::Client() : _lastping(std::time(0))
{
}

Client::~Client()
{
}

bool Client::accept(const net::Socket& s)
{
	return _socket.accept(s);
}

bool Client::send(const char* data, uint32 size) const
{
	return _socket.send(data, size);
}

bool Client::receive(void* data, uint32 size)
{
	ssize_t r = _socket.receive(data, size - 1);
	if(r == 0)
		return false;
	else if(r < 0)
		return false; // TODO: Handle errors
	reinterpret_cast<byte*>(data)[r] = '\0';
	return true;
}

bool Client::ping()
{
	_lastping = Server::currenttime();
	std::cout << "Pinging client" << std::endl;
	return _socket.send("PING 123\n", 10);
}
