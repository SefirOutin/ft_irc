#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.class.hpp"
#include "ft_irc.hpp"

class Client;

class Server
{
	public:
		Server(int port, const std::string &password);
		~Server();

		int startServer();
		int run();

	private:
		int 						_sockFd;
		sockaddr_in					_sockAddr;
		std::vector<struct pollfd>	_fds;
		std::map<int, Client>		_clients;
		std::string					_password;
		
		void	socketOpt();
		int		acceptConnections();
		void	closeConnection(int clientFd);
		void	receivedData(int clientFd);
		int		getCmd(std::string buff, int clientFd);
		int		handleData(std::string cmd, std::string arg, int clientFd);

};

#endif