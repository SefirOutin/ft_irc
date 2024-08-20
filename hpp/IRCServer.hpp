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
	void closeClientConnection(pollfd &pfd, std::vector<pollfd> &pollfds, size_t index);

private:
	int serverSocket;
	int port;
	bool running;
	sockaddr_in _sockAddr;
	std::vector<struct pollfd> _fds;
	IRCCommandParser commandParser;
	std::map<int, IRCClientHandler *> clients;
	int createServerSocket();
	void acceptNewClient(std::vector<pollfd> &pollfds);
	void handleClientMessage(pollfd &pfd, std::vector<pollfd> &pollfds, size_t index);
};

#endif