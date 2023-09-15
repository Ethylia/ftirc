#include "irccommands.hpp"
#include "client.hpp"
#include "channel.hpp"
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
		{
			client->send(":" + Server::host() + " 461 " + client->nick() + " OPER :Not enough parameters.\r\n");
			return false;
		}

		if(!client->oper(command.params[0], command.params[1]))
		{
			client->send(":" + Server::host() + " 491" + client->nick() + " :Invalid oper credentials\r\n");
			return false;
		}

		client->send(":" + client->host() + " MODE " + client->nick() + " :+o\r\n");
		client->send("381 " + client->nick() + " :You are now an IRC operator\r\n");

		return true;
	}

	static int parseMode(char mode)
	{
		for(size_t i = 0; i < std::char_traits<char>::length(Client::USERMODES); ++i)
			if(mode == Client::USERMODES[i])
				return 1 << i;
		return Client::MODE_UNKNOWN;
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
			if(command.params[1][0] != '+' && command.params[1][0] != '-')
				return false;
			std::string modestr;
			bool unknown = false;
			while(i < command.params[1].size())
			{
				size_t j;
				modestr += command.params[1][i];
				for(j = i + 1; command.params[1][j] && command.params[1][j] != '+' && command.params[1][j] != '-'; ++j)
				{
					int r = parseMode(command.params[1][j]);
					if(r == Client::MODE_UNKNOWN)
						unknown = true;
					else if(command.params[1][i] == '+' && client->addmode(r))
						modestr += command.params[1][j];
					else if(client->delmode(r))
						modestr += command.params[1][j];
				}
				i = j;
			}
			// send a message if there were any unknown modes
			if(unknown)
				client->send("501 " + client->nick() + " :Unknown MODE flag\r\n");
			// send a MODE message if there were any modes changed
			if(modestr.find_first_not_of("+-") != std::string::npos)
				client->send(":" + Server::host() + " MODE " + target->nick() + " " + modestr + "\r\n");
		}
		return true;
	}

	bool join(const Command& command, Client* client)
	{
		if(command.params.size() < 1)
			return false;
		// check if the channel name is valid
		std::string channelPrefixes = "#&+!";
		if(command.params[0].size() > 1 || channelPrefixes.find(command.params[0][0]) == std::string::npos)
		{
			client->send("ERROR :Invalid channel name\r\n");
			return false;
		}
		// check if the channel exists
		Channel* channel = Server::channel(command.params[0]);
		if(!channel)
		{
			channel = Server::createChannel(command.params[0], client);
			if(!channel)
			{
				client->send("ERROR :Channel creation failed\r\n");
				return false;
			}
		}
		channel->addUser(client);
		client->send("JOIN " + channel->getChannelName() + "\r\n");
		return true;
	}
}
