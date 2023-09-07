#include "server.hpp"

#include <iostream>

#include "net/address.hpp"

net::Socket Server::_asocket;
Client* Server::_newclient = 0;
std::vector<Client*> Server::_clients;
std::vector<pollfd> Server::_pollfds;

const uint32 Server::_QUEUE_SIZE = 64;

Server::Server()
{
	init();
}

Server::Server(const Server& obj)
{
	(void)obj; // nuh uh
}

Server::~Server()
{
	std::cout << "Server destructor" << std::endl;
	delete _newclient;
}

bool Server::init()
{
	_clients.reserve(16);
	_pollfds.reserve(16);
	if(!_asocket.create(net::ipv4, net::tcp))
	{
		std::cerr << "Failed to open accepting socket" << std::endl;
		return false;
	}
	const in_addr addr = INITADDR;
	if(!_asocket.bind(addr, 8080))
	{
		std::cerr << "Failed to bind accepting socket to port 8080" << std::endl;
		return false;
	}
	if(!_asocket.listen(_QUEUE_SIZE))
	{
		std::cerr << "Failed to enable listen queue on accepting socket" << std::endl;
		return false;
	}
	pollfd pfd = {.fd = _asocket, .events = POLLIN, .revents = 0};
	_pollfds.push_back(pfd);
	return true;
}

bool Server::run()
{
	_newclient = new Client();
	while(true)
	{
		if(poll(_pollfds.data(), _pollfds.size(), -1) == -1)
		{
			std::cerr << "Poll error" << std::endl;
			return false;
		}
		if(_pollfds[0].revents & POLLIN && _newclient->accept(_asocket))
		{
			std::cout << "Accepted connection" << std::endl;
			_clients.push_back(_newclient);
			_newclient = new Client();
			pollfd pfd = {.fd = *_newclient, .events = POLLIN, .revents = 0};
			_pollfds.push_back(pfd);
		}
	}
	return false;
}
