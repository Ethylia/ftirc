#include "irccommands.hpp"
#include "client.hpp"

#include "server.hpp"

namespace Command
{
	bool pass(const Command &command, Client *client)
	{
		if(!client->password(command.params[0]))
		{
			client->send("ERROR :Password incorrect\r\n");
			return false;
		}
		return true;
	}

	bool nick(const Command &command, Client *client)
	{
		if(Server::client(command.params[0]))
		{
			client->send("ERROR :Nickname already in use\r\n");
			return false;
		}
		if(!client->setnick(command.params[0]))
		{
			client->send("ERROR :Invalid nickname\r\n");
			return false;
		}
		return true;
	}

	bool user(const Command& command, Client* client)
	{
		// check if already registered
		if(client->registered())
		{
			client->send("You are already registered\r\n");
			return false;
		}
		// check parameters numbers
		if(command.params.size() < 4)
		{
			client->send("ERROR :Not enough parameters\r\n");
			return false;
		}
		else if(command.params.size() > 4)
		{
			client->send("ERROR :Too many parameters\r\n");
			return false;
		}
		// check nick is set
		if(client->nick().empty())
		{
			client->send("ERROR :No nickname given\r\n");
			return false;
		}
		// set user
		if(!client->setuser(command.params[0], command.params[1], command.params[3]))
		{
			client->send("ERROR :Invalid username\r\n");
			return false;
		}
		// send welcome message
		client->send("Welcome to the Internet Relay Chat\r\n");

		return true;
	}

	bool privmsg(const Command& command, Client* client)
	{
		// check parameters numbers
		if(command.params.size() < 2)
		{
			client->send("ERROR :Not enough parameters\r\n");
			return false;
		}
		// check msgReceiver
		const Client* const msgTarget = Server::client(command.params[0]);
		if(!msgTarget)
		{
			client->send("ERROR :No such nick\r\n");
			return false;
		}
		if(!msgTarget->registered())
		{
			client->send("ERROR :msgTarget is not registered\r\n");
			return false;
		}
		// send message
		std::string msg = ":" + client->nick() + "!" + client->user() + "@" + client->host() + " PRIVMSG " + msgTarget->nick();
		// we splitted the message too, so we need to reassemble it
		for(size_t i = 1; i < command.params.size() - 1; i++)
		{
			msg += " ";
			msg += command.params[i];
		}
		msg += "\r\n";
		msgTarget->send(msg.c_str());
		return true;
	}

	bool quit(const Command& command, Client* client)
	{
		(void)command;
		(void)client;
		return true;
	}

	bool ping(const Command& command, Client* client)
	{
		if(command.params.size() < 1)
			return false;
		return Server::send("PONG " + command.params[0] + "\r\n", client);
	}

	bool pong(const Command& command, Client* client)
	{
		(void)client;
		if(command.params.size() < 1)
			return false;
		return true;
	}
}