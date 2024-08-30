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
#include "IRCCmds.hpp"
#include "IRCError.hpp"

class	IRCClient;
class	IRCCommandHandler;
class	IRCChannel;

class	IRCServer
{
	public:
		IRCServer(int port, const std::string &password);
		~IRCServer();

		const std::string 						&getPass() const;
		const std::map<int, IRCClient>			&getClients() const;
		const std::map<std::string, IRCChannel>	&getChannels() const;
		
		int 	startServer();
		int 	run();
		void	parseCmds(const std::string &message, IRCClient &client);
		void	closeConnection(int clientFd);
		
		void	newChannel(const std::string &name, IRCClient &Op);
		void	removeChannel(std::string name);
		void	newConnectionToChannel(const std::string &name, IRCClient &client);
		void	removeClientFromChannel(const std::string &name, int clientFd);

	private:
		int											_sockFd;
		std::string									_password;
		sockaddr_in									_sockAddr;
		std::vector<struct pollfd>					_fds;
		std::map<int, IRCClient>					_clients;
		std::map<std::string, IRCCommandHandler *>	_cmds;
		std::map<std::string, IRCChannel>			_channels;
		
		void	socketOpt();
		int		acceptConnections();
};

#endif