#include <iostream>

#include <signal.h>

#include "server/server.hpp"

void signal_handler(int signum)
{
	Server::shutdown();
	exit(signum);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Server server;

	struct sigaction sigact = {.sa_handler = signal_handler};
	sigaction(SIGINT, &sigact, NULL);

	if(!Server::init(8080, "password"))
	{
		std::cerr << "Failed to initialize server" << std::endl;
		return 1;
	}
	Server::run();
	Server::shutdown();

	return 0;
}
