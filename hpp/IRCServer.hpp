#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

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
#include "IRCClient.hpp"
#include "IRCCommandHandler.hpp"
#include "IRCChannel.hpp"

class IRCClient;
class IRCCommandHandler;

class IRCServer
{
public:
	IRCServer() {};
	IRCServer(int port, const std::string &password);
	~IRCServer();

	const std::string &getPass() const;
	const std::map<int, IRCClient> &getClients() const;
	
	int startServer();
	int run();
	void parseCmds(const std::string &message, IRCClient &client);
	void closeConnection(int clientFd);

private:
	int _sockFd;
	sockaddr_in _sockAddr;
	std::vector<struct pollfd> _fds;
	std::map<int, IRCClient> _clients;
	std::map<std::string, IRCCommandHandler *> _cmds;
	//std::map<std::string, IRCChannel>	_channels;
	
	std::string _password;
	void socketOpt();
	int acceptConnections();
};

#endif