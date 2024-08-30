#include "IRCServer.hpp"

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "Usage: ./ircserv [port] [password]\n";
		return (1);
	}
	
	IRCServer	server(atoi(av[1]), av[2]);
	server.startServer();
	server.run();
	return (0);
	(void) ac;
}