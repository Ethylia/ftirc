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

	template<void (Client::*func)(int)>
	static bool parseModes(std::string& s, const char* const modes, Client* client)
	{
		bool allKnown = true;
		for(size_t i = 0; i < s.size(); ++i)
		{
			bool found = false;
			for(size_t j = 0; j < std::char_traits<char>::length(modes); ++j)
				if(s[i] == modes[j])
				{
					found = true;
					(client->*func)(1 << j);
					break;
				}
			if(!found)
			{
				unknownModes = false;
				s.erase(i, 1);
				--i;
			}
		}
		return allKnown;
	}

	bool mode(const Command& command, Client* client)
	{
		if(command.params.size() < 2)
			return false;
		
		if(command.params[0][0] != '#')
		{ // if the first parameter is not a channel, it's a user
			Client* target = Server::client(command.params[0]);
			if(!target)
			{ // find the user and check if it exists
				client->send("ERROR :No such nick\r\n");
				return false;
			}
			// parse the modes
			size_t i = 0;
			bool unknownModes = false;
			std::string modes;
			if(command.params[1][i] == '+')
			{ // if there is a '+', fetch all the modes to be added after it
				size_t pos = command.params[1].find_first_not_of(Client::USERMODES, i + 1);
				if(pos - i > 1) // chop a substring from the + to the closest non-mode character
					modes += command.params[1].substr(i, pos - i);
				// add all the modes to the client and erase invalid ones from the mode string
				for(size_t j = i + 1; j < pos; ++j)
					if(!target->addmode(command.params[1][j]))
					{
						unknownModes = true;
						modes.erase(j, 1);
					} 
				i = pos;
			}
			if(command.params[1][i] == '-')
			{ // next we do the same for the '-' modes that come after the '+' ones or at the start
				size_t pos = command.params[1].find_first_not_of(Client::USERMODES, i + 1);
				if(pos - i > 1)
					modes += command.params[1].substr(i, pos - i);
				for(size_t j = i; j < pos; ++j)
					if(!target->delmode(command.params[1][j]))
					{
						unknownModes = true;
						modes.erase(j, 1);
					}
			}
			// send a message if there were any unknown modes
			if(unknownModes)
				client->send("501 " + client->nick() + " :Unknown MODE flag\r\n");
			// send a MODE message if there were any modes changed
			if(!modes.empty())
				client->send("MODE " + target->nick() + " " + modes + "\r\n");
		}
		return true;
	}
}
