#include "client.hpp"

#include "server.hpp"
#include "command.hpp"

#include <iostream>

Client::Client() : flagDisconnect(false), _lastping(0), _lastpinged(0), _passworded(false), _registered(false)
{
}

Client::~Client()
{
}

bool Client::accept(const net::Socket& s)
{
	_lastping = Server::currenttime();
	return _socket.accept(s);
}

bool Client::send(const char* data, uint32 size) const
{
	// lastping = Server::currenttime();
	return _socket.send(data, size);
}

bool Client::receive()
{
	_lastping = Server::currenttime();
	char buffer[1024];
	ssize_t r;
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

	_lastpinged = Server::currenttime();
	std::cout << "Pinging client" << std::endl;
	return _socket.send("PING 123\r\n", 10);
}

bool Client::password(const std::string& pass)
{
	if(pass == Server::password())
		_passworded = true;
	return _passworded;
}

bool Client::setnick(const std::string& nick)
{
	if(nick.empty() || nick.size() > 9)
		return false;
	_nick = nick;
	return true;
}

bool Client::setuser(const std::string& user, const std::string& host, const std::string& realname)
{
	if(user.empty() || host.empty() || realname.empty())
		return false;
	if(user.size() > 9)
		return false;

	// if () {} // Check hostname too?
	
	_user = user;
	_host = host;
	_realname = realname;

	_registered = true;
	return true;
}