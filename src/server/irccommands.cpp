#include "irccommands.hpp"
#include "client.hpp"

#include "server.hpp"

namespace Command
{
	bool pass(const Command &command, Client *client)
	{
		if(!client->password(command.params[0]))
		{
			client->send("ERROR :Password incorrect\r\n", 26);
			return false;
		}
	}

	bool nick(const Command &command, Client *client)
	{
		if(Server::client(command.params[0]))
		{
			client->send("ERROR :Nickname already in use\r\n", 30);
			return false;
		}
		if(!client->setnick(command.params[0]))
		{
			client->send("ERROR :Invalid nickname\r\n", 23);
			return false;
		}
		return true;
	}

	bool user(const Command& command, Client* client)
	{
		// check parameters numbers
		if (command.params.size() < 3)
		{
			client->send("ERROR :Not enough parameters\r\n", 29);
			return false;
		}
		else if (command.params.size() > 3)
		{
			client->send("ERROR :Too many parameters\r\n", 28);
			return false;
		}
		// check passworded
		if (client->passworded())
		{
			client->send("ERROR :Password incorrect\r\n", 26);
			return false;
		} 
		// check nick is set
		if (client->nick().empty())
		{
			client->send("ERROR :No nickname given\r\n", 24);
			return false;
		}
		// check if already registered
		if(client->registered())
		{
			client->send("You are already registered\r\n", 27);
			return false;
		}
		// set user
		if(!client->setuser(command.params[0], command.params[1], command.params[2]))
		{
			client->send("ERROR :Invalid username\r\n", 23);
			return false;
		}
		// send welcome message
		if(client->registered())
			client->send("Welcome to the Internet Relay Chat\r\n", 36);

		return true;
	}

	bool privmsg(const Command& command, Client* client)
	{
		// check parameters numbers
		if (command.params.size() < 2)
		{
			client->send("ERROR :Not enough parameters\r\n", 29);
			return false;
		}
		// check if registered
		if(!client->registered())
		{
			client->send("You are not registered\r\n", 27);
			return false;
		}
		// check msgReceiver
		const Client* msgTarget = Server::client(command.params[0]);
		if(!msgTarget)
		{
			client->send("ERROR :No such nick\r\n", 20);
			return false;
		}
		if(!msgTarget->registered())
		{
			client->send("ERROR :msgTarget is not registered\r\n", 35);
			return false;
		}
		// send message
		std::string msg = ":" + client->nick() + "!" + client->user() + "@" + client->host() + " PRIVMSG " + msgTarget->nick();
		// we splitted the message too, so we need to reassemble it
		for (size_t i = 1; i < command.params.size() - 1; i++)
		{
			msg += " ";
			msg += command.params[i];
		}
		msg += "\r\n";
		msgTarget->send(msg.c_str(), msg.size());
		return true;
	}
}