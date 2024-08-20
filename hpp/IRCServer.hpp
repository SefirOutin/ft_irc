#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "IRCCommandParser.hpp"
#include "IRCCommandHandler.hpp"
#include "IRCClientHandler.hpp"

class IRCClientHandler;

class IRCServer
{
	public:
		IRCServer(int port, const std::string &pass);
		~IRCServer();
		void start();
		void stop();

	protected:
		std::string _password;
		void closeClientConnection(int clientFd);

	private:
		int 								_serverSocket;
		bool								_running;
		std::vector<struct pollfd>			_fds;
		IRCCommandParser					_commandParser;
		std::map<int, IRCClientHandler *>	_clients;

		void createServerSocket(int port);
		void acceptNewClient();
		void handleClientMessage(pollfd &pfd, std::vector<pollfd> &pollfds, size_t index);
};

#endif