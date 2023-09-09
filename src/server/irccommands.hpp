#pragma once

#include "command.hpp"

namespace Command
{
	bool pass(const Command& command, Client* client);
	bool nick(const Command& command, Client* client);
	bool user(const Command& command, Client* client);
	bool privmsg(const Command& command, Client* client);
	bool quit(const Command& command, Client* client);
	bool ping(const Command& command, Client* client);
	bool pong(const Command& command, Client* client);
}