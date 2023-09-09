#include "command.hpp"
#include "client.hpp"

void Command::parse(std::string command, Client* client)
{
    const std::string cmdList[] = {"PASS", "NICK", "USER", "PRIVMSG"};
    for (size_t i = 0; i < sizeof(cmdList)/sizeof(std::string); i++)
    {
        if (command.compare(cmdList[i]) == 0)
        {
            Command command;
            command.type = (Type)i;
            
            
            execute(command, client);
        }
    }
}

std::vector<std::string> Command::splitCmd(std::string command)
{
    std::vector<std::string> splitCmd;
	std::string::size_type pos = command.find(" " , 0);
	std::string::size_type newpos = command.find("\r\n", 0);
	std::string::size_type colone = command.find(":", 0);

	int i = 0;
	while ((pos != std::string::npos || newpos != std::string::npos) && pos < colone)
	{
		if (pos > newpos)
		{
			splitCmd.push_back(command.substr(i, newpos - i));
			i = newpos + 2;
		}
		else
        {
			splitCmd.push_back(command.substr(i, pos - i));
			i = pos + 1;
		}
		pos = command.find(" ", i);
		newpos = command.find("\r\n", i);
	}
	if (colone != std::string::npos)
		splitCmd.push_back(command.substr(colone, command.length() - colone - 2));
	else
		splitCmd.push_back(command.substr(i, command.length() - i - 2));
	return splitCmd;
}

void Command::execute(const Command& command, Client* client)
{
}