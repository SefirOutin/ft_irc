#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include "IRCClientHandler.hpp"
#include "IRCCmds.hpp"
#include "ft_irc.hpp"

class	IRCClientHandler;
class	IRCCommandHandler;

class	IRCServer
{
	public:
		IRCServer(int port, const std::string &password);
		~IRCServer();

		const std::string	getPass() const;

		int startServer();
		int run();

		bool	nickAlreadyInUse(std::string arg, int clientFd);
		void 	parseCommand(const std::string& buffer, IRCClientHandler client);

	private:
		int _sockFd;
		sockaddr_in _sockAddr;
		std::vector<struct pollfd> _fds;
		std::map<int, IRCClientHandler> _clients;
		std::map<std::string, IRCCommandHandler *> _cmds;
		std::string _password;

		void socketOpt();
		int acceptConnections();
		void closeConnection(int clientFd);
		void receivedData(int clientFd);
		int getCmd(std::string buff, int clientFd);
		int handleData(std::string cmd, std::string arg, int clientFd);
};

#endif