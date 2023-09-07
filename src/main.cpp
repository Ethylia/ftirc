#include <iostream>

#include "server/server.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Server server;

	if(!Server::init())
	{
		std::cerr << "Failed to initialize server" << std::endl;
		return (1);
	}
	Server::run();

	return (0);
}
