#include "client.hpp"

#include "server.hpp"
#include "command.hpp"

#include <iostream>

Client::Client() : lastping(0)
{
}

Client::~Client()
{
}

bool Client::accept(const net::Socket& s)
{
	lastping = Server::currenttime();
	return _socket.accept(s);
}

bool Client::send(const char* data, uint32 size) const
{
	// lastping = Server::currenttime();
	return _socket.send(data, size);
}

bool Client::receive()
{
	lastping = Server::currenttime();
	char buffer[1024];
	uint32 r;
	if((r = _socket.receive(buffer, 1023)) < 1)
		return false;

	do
	{
		buffer[r] = '\0';
		_data += buffer;
		size_t pos;
		if((pos = _data.find("\r\n")) != std::string::npos)
		{
			std::cout << "Received: " << _data.substr(0, pos) << std::endl;
			Command::parse(_data.substr(0, pos), this);
			_data.erase(0, pos + 2);
		}
	} while ((r = _socket.receive(buffer, 1023)) > 0);
	
	return true;
}

bool Client::ping()
{

	// lastping = Server::currenttime();
	std::cout << "Pinging client" << std::endl;
	return _socket.send("PING 123\r\n", 10);
}
