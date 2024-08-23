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

class IRCClient;
class IRCCommandHandler;
class IRCServer
{
public:
	IRCServer() {};
	IRCServer(int port, const std::string &password);
	~IRCServer();
	int startServer();
	int run();
	const std::string &getPass() const;
	void parseCmds(const std::string &message, IRCClient server);
	const std::map<int, IRCClient> &getClients() const;

private:
	int _sockFd;
	sockaddr_in _sockAddr;
	std::vector<struct pollfd> _fds;
	std::map<int, IRCClient> _clients;
	std::map<std::string, IRCCommandHandler *> _cmds;
	std::string _password;
	void socketOpt();
	int acceptConnections();
	void closeConnection(int clientFd);
	void receivedData(int clientFd);
};

#endif