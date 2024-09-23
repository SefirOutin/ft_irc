#include "IRCServer.hpp"
#include <string>
#include <cstdlib>

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "Usage: ./ircserv [port] [password]\n";
		return (1);
	}
	
	std::string port_str(av[1]);
	if (!is_number(port_str))
	{
		std::cout << "Error: Port must be a number\n";
		return (1);
	}

	int port = std::atoi(port_str.c_str());
	IRCServer server(port, av[2]);
	if (server.startServer())
		return (1);
	server.run();
	return (0);
}