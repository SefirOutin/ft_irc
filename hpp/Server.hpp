#pragma once

#include "Client.hpp"
#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <poll.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <sstream>
#include <algorithm>

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
		
		int		acceptConnections();
		void	receivedData(int clientFd, int index);
		int		getCmd(std::string buff, int clientFd);
		int		handleData(std::string cmd, std::string arg, int clientFd);
		bool	nickAlreadyInUse(std::string arg,int clientFd);
		void	PASS(std::string arg, int clientFd);
		void	NICK(std::string arg, int clientFd);
		void	USER(std::string arg, int clientFd);
};