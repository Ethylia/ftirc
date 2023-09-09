#pragma once

#include <vector>
#include <string>
#include <poll.h>

#include "common.hpp"
#include "net/net.hpp"
#include "client.hpp"

class Server
{
public:
	Server();
	~Server();

	static bool init(uint16 port, const char* password);
	static bool run();
	static void shutdown();

	static bool accept();
	static bool receive(uint64 id);
	static void disconnect(uint64 id);

	static time_t currenttime() { return _currenttime; }

private:
	Server(const Server& obj);
	Server& operator=(const Server& obj);

	static time_t _currenttime;

	static uint16 _port;
	static std::string _password;

	static net::Socket _asocket;
	static Client* _newclient;
	static std::vector<Client*> _clients;
	static std::vector<pollfd> _pollfds;

	static const uint32 _QUEUE_SIZE;
};
