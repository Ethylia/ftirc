#include "irccommands.hpp"
#include "client.hpp"

#include "server.hpp"

#include <sstream>

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
		std::stringstream ss;
		ss << Server::clientCount();
		std::string clientCount = ss.str();

		client->send("001 " + client->nick() + " :Welcome to the Internet Relay Chat\r\n");
		client->send("002 " + client->nick() + " :Your host is " + Server::NAME + ", running version " + Server::VERSION + "\r\n");
		client->send("003 " + client->nick() + " :This server was created sometime ago" + "\r\n");
		client->send("004 " + client->nick() + " " + Server::NAME + "-" + Server::VERSION + " " + "o o" + "\r\n");
		client->send("005 " + client->nick() + " PREFIX=(ov)@+ CHANTYPES=# CHANMODES=,,," + ":are supported by this server\r\n");

		client->send("251 " + client->nick() + " :There are " + clientCount + " users and 0 services on 1 server\r\n");

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
		std::string msg = ":" + client->nick() + "!" + client->user() + "@" + client->host() + " PRIVMSG " + msgTarget->nick() + ((command.params[1][0] == ':') ? " " : " :") + command.params[1] + "\r\n";

		msgTarget->send(msg.c_str());
		return true;
	}

	bool quit(const Command& command, Client* client)
	{
		if(command.params.size() < 1)
			return false;
		client->send("ERROR :Quit: " + command.params[0] + "\r\n");
		client->flagDisconnect = true;
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

	bool cap(const Command& command, Client* client)
	{
		if(command.params.size() < 1)
			return false;
		if(command.params[0] == "LS")
			client->send("CAP * LS\r\n");
		return true;
	}

	bool oper(const Command& command, Client* client)
	{
		if(command.params.size() < 2)
			return false;

		if(!client->oper(command.params[0], command.params[1]))
		{
			client->send("ERROR :Invalid oper credentials\r\n");
			return false;
		}
		client->send("381 " + client->nick() + " :You are now an IRC operator\r\n");

		//std::string msg = ":" + client->nick() + "!" + client->user() + "@" + client->host() + " MODE " + channel->name() + " +o " + opUser->nick() + "\r\n";

		return true;
	}

	bool mode(const Command& command, Client* client)
	{
		const char* const USERMODES = "iosv";
		if(command.params.size() < 2)
			return false;

		std::string addmodes;
		std::string delmodes;
		// gather all the added and removed modes in two strings
		for(size_t i = 1; i < command.params.size(); ++i)
			if(command.params[i][0] == '+')
				for(size_t j = 1; j < command.params[i].size(); ++j)
					addmodes += command.params[i][j];
			else if(command.params[i][0] == '-')
				for(size_t j = 1; j < command.params[i].size(); ++j)
					delmodes += command.params[i][j];
		
		// remove all unknown modes and send a message if there were any
		// need to erase so the MODE reply has the correct modes
		size_t p = addmodes.find_first_not_of(USERMODES);
		size_t p2 = delmodes.find_first_not_of(USERMODES);
		bool unknownModes = (p != std::string::npos || p2 != std::string::npos);
		while(p != std::string::npos)
		{
			addmodes.erase(p, 1);
			p = addmodes.find_first_not_of(USERMODES);
		}
		while(p2 != std::string::npos)
		{
			delmodes.erase(p2, 1);
			p2 = delmodes.find_first_not_of(USERMODES);
		}

		if(unknownModes)
			client->send("501 " + client->nick() + " :Unknown MODE flag\r\n");

		// apply all modes to client
		for(size_t i = 0; i < addmodes.size(); ++i)
			for(size_t j = 0; j < std::char_traits<char>::length(USERMODES); ++j)
				if(addmodes[i] == USERMODES[j])
				{
					client->addmode(1 << j);
					break;
				}
		for(size_t i = 0; i < delmodes.size(); ++i)
			for(size_t j = 0; j < std::char_traits<char>::length(USERMODES); ++j)
				if(delmodes[i] == USERMODES[j])
				{
					client->delmode(1 << j);
					break;
				}
		// MODE reply with all known modes that were changed
		if(!addmodes.empty() || !delmodes.empty())
			client->send("MODE " + client->nick() + ((!addmodes.empty()) ? (" +" + addmodes) : (" ")) + ((!delmodes.empty()) ? (" -" + delmodes) : (" ")) + "\r\n");

		return true;
	}
}
