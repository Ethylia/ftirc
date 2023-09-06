#pragma once

#include <vector>

#include "net/sockets.hpp"
#include "client.hpp"

class Server
{
public:
	Server();

	Server(const Server& obj);
	Server& operator=(const Server& obj) = delete;

	~Server();

	static bool init();

private:
	static net::Socket _asocket;
	static std::vector<Client*> _clients;
}
