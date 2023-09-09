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
	bool receive();
	bool send(const char* data, uint32 size) const;
	bool ping();

	const std::string& data() const { return _data; }

	std::string nick;
	std::string user;
	std::string host;
	std::string realname;

	std::time_t lastping;
	bool flagDisconnect;

private:
	Client(const Client& obj);
	Client& operator=(const Client& obj);

	std::string _data;

	net::Socket _socket;
};
