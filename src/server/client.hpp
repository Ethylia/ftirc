#pragma once

#include "net/socket.hpp"

#include <string>
#include <ctime>

class Client
{
public:
	Client();
	~Client();

	operator const int&() const { return _socket; }

	bool accept(const net::Socket& s);
	bool receive(void* data, uint32 size);
	bool send(const char* data, uint32 size) const;
	bool ping();

	std::string name;

	time_t lastping() const { return _lastping; }

private:
	Client(const Client& obj);
	Client& operator=(const Client& obj);

	std::time_t _lastping;

	net::Socket _socket;
};
