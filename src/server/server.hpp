#pragma once

#include <vector>
#include <poll.h>

#include "common.hpp"
#include "net/net.hpp"
#include "client.hpp"

class Server
{
public:
	Server();
	~Server();

	static bool init();
	static bool run();

private:
	Server(const Server& obj);
	Server& operator=(const Server& obj);

	static net::Socket _asocket;
	static Client* _newclient;
	static std::vector<Client*> _clients;
	static std::vector<pollfd> _pollfds;

	static const uint32 _QUEUE_SIZE;
};
