#ifndef IRCCLIENT_HPP
#define IRCCLIENT_HPP

#include "IRCServer.hpp"

class IRCServer;

class IRCClient
{
	public:
		// IRCClient() {};
		IRCClient(int fd, IRCServer *server);
		IRCClient(const IRCClient &other);
		~IRCClient();

		IRCClient &operator=(const IRCClient &other);

		IRCClient	getClient(const std::string &nick);
		const std::string	&getNick() const;
		const std::vector<std::string>	getUser() const;
		int	getFd() const;
		bool	getWelcom();
		bool	isConnected() const;
		std::string	getClientInfos();

		void 	setNick(std::string nick);
		void 	setUser(const std::string &user);
		void 	setConnected(bool status);
		void 	setWelcom(bool status);

		bool	checkPass(const std::string &password) const;
		void	sendMessage(const std::string &msg) const;
		void	receiveMessages();
		bool	nickAlreadyInUse(std::string arg, int clientFd);
		int		leaveChannel(const std::string &name);
		void	joinChannel(const std::string &name);
		void	createChannel(const std::string &name);
		bool	channelNameAlreadyInUse(const std::string &name);

	private:
		IRCServer *_server;
		int _fd;
		bool _connected;
		bool _sendWelcom;
		std::string _nick;
		std::vector<std::string> _user;
};

#endif