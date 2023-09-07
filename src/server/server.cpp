#include "server.hpp"

#include <iostream>

#include "net/address.hpp"

uint16 Server::_port = 8080;
std::string Server::_password;
net::Socket Server::_asocket;
Client* Server::_newclient = 0;
std::vector<Client*> Server::_clients;
std::vector<pollfd> Server::_pollfds;

const uint32 Server::_QUEUE_SIZE = 64;

Server::Server()
{
}

Server::Server(const Server& obj)
{
	(void)obj; // nuh uh
}

Server::~Server()
{
	shutdown();
}

bool Server::init(uint16 port, const char* password)
{
	_port = port;
	_password = password;
	_clients.reserve(16);
	_pollfds.reserve(16);
	if(!_asocket.create(net::ipv4, net::tcp))
	{
		std::cerr << "Failed to open accepting socket" << std::endl;
		return false;
	}
	const in_addr addr = INITADDR;
	if(!_asocket.bind(addr, _port))
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
			pollfd pfd = {.fd = *_newclient, .events = POLLIN, .revents = 0};
			_pollfds.push_back(pfd);
			_newclient = new Client();
		}
		std::cout << "Poll returned" << std::endl;
		for(uint64 i = 1; i < _pollfds.size(); ++i)
		{
			if(_pollfds[i].revents & POLLIN)
			{
				byte data[1024];
				if(!_clients[i - 1]->receive(data, 1024))
				{
					std::cout << "Client disconnected" << std::endl;
					delete _clients[i - 1];
					_clients.erase(_clients.begin() + i - 1);
					_pollfds.erase(_pollfds.begin() + i);
					--i;
				}
				else
				{
					std::cout << "Received data: " << data << std::endl;
				}
			}
		}
	}
	return false;
}

void Server::shutdown()
{
	std::cout << "Server destructor" << std::endl;
	delete _newclient;
	_newclient = 0;
}
