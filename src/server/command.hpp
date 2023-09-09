#pragma once

#include <iostream>
#include <vector>

class Client;

namespace Command {
	enum Type {
		PASS,
		NICK,
		USER,
		PRIVMSG,
		QUIT,
		PING,
		PONG,
		UNKNOWN
	};
	struct Prefix
	{
		std::string nick;
		std::string user;
		std::string host;
	};
	struct Command {
		Type type;
		Prefix prefix;
		std::vector<std::string> params;
	};

	void parse(std::string command, Client* client);
	void execute(const Command& command, Client* client);
	std::vector<std::string> splitCmd(std::string command);
}
