#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# include <algorithm>
# include <arpa/inet.h>
# include <cstdlib>
# include <ctime>
# include <errno.h>
# include <iostream>
# include <map>
# include <netinet/in.h>
# include <poll.h>
# include <sstream>
# include <string.h>
# include <string>
# include <sys/ioctl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <unistd.h>
# include <vector>
# include "IRCChannel.hpp"
# include "IRCClient.hpp"
# include "IRCCmds.hpp"
# include "IRCCommandHandler.hpp"
# include "IRCError.hpp"

class	IRCClient;
class	IRCCommandHandler;
class	IRCChannel;

class IRCServer
{
  public:
	IRCServer(int port, const std::string &password);
	~IRCServer();
  
	const std::string &getPass() const;
	const std::map<int, IRCClient> &getClients() const;
	const std::map<std::string, IRCChannel> &getChannels() const;

	int startServer();
	int run();
	void parseCmds(const std::string &message, IRCClient &client);
	void closeConnection(int clientFd);

	void newChannel(const std::string &name, IRCClient &Op);
	void removeChannel(std::string name);
	void newConnectionToChannel(const std::string &name, IRCClient &client);
	void	removeClientFromChannel(const std::string &name, int clientFd);
	void sendToChannel(const std::string &message, int senderFd,
			const std::string &chanName);

	void	setTopic(const std::string &chanName, const std::string &topic);
	IRCChannel* findChannel(const std::string& chanName);
	void	setMode(const std::string &chanName, const std::string &mode);
	bool	inMode(const std::string &chanName, const std::string &mode);
	void	whiteList(const std::string &nick, const std::string &chanName);
	bool	isWhiteListed(const std::string &nick, const std::string &chanName);
	void	changeOpe(const std::string &chanName, const std::string &nick, bool op);
	void	setKey(const std::string &chanName, const std::string &key);
  
  private:
	int _sockFd;
	std::string _password;
	sockaddr_in _sockAddr;
	std::vector<struct pollfd> _fds;
	std::map<int, IRCClient> _clients;
	std::map<std::string, IRCCommandHandler *> _cmds;
	std::map<std::string, IRCChannel> _channels;

	void socketOpt();
	int acceptConnections();
};

#endif