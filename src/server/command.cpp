#include "command.hpp"

#include "irccommands.hpp"
#include "client.hpp"

bool Command::parse(std::string command, Client* client)
{
	Command cmd;
	cmd.type = UNKNOWN;
    const char* const cmdList[] = {"PASS", "NICK", "USER", "PRIVMSG", "QUIT", "PING", "PONG"};

	if(command.empty())
		return false;

	size_t i = 0;
	if(command[0] == ':')
	{
		size_t pos = command.find(" ", 0);
		if(pos == std::string::npos)
			return false;
		i = pos + 1;
	}

	size_t y = 0;
    for (; y < sizeof(cmdList)/sizeof(char*); ++y)
		if(command.compare(i, std::char_traits<char>::length(cmdList[y]), cmdList[y]) == 0)
		{
			cmd.type = (Type)(y + 1);
			break;
		}
	if(cmd.type == UNKNOWN)
		return false; // TODO: send error message or more
	i = command.find(" ", i + std::char_traits<char>::length(cmdList[y]));
	while(i++ < command.size())
	{
		size_t pos = command.find(" ", i);
		cmd.params.push_back(command.substr(i, pos - i));
		i = pos;
	}
	return execute(cmd, client);
}

bool Command::execute(const Command& command, Client* client)
{
	bool (* const cmdList[])(const Command&, Client*) = {0, pass, nick, user, privmsg, quit, ping, pong};
	if(!client->registered())
	{
		if(!client->passworded() && command.type != PASS)
			return false;
		if(command.type != NICK && command.type != USER)
			return false;
	}
	return cmdList[command.type](command, client);
}
