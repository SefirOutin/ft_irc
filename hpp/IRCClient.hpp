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

		IRCClient	&operator=(const IRCClient &other);

		const std::string				&getNick() const;
		void						 	setNick(std::string nick);
		const std::vector<std::string>	&getUser() const;
		void 							setUser(const std::string &user);
		const bool						&getWelcom() const;
		void 							setWelcom(bool status);
		const int						&getFd() const;
		const bool						&isConnected() const;
		void 							setConnected(bool status);
		const IRCClient					&getClient(const std::string &nick) const;
		std::string						getClientInfos() const;
		const std::map<int, IRCClient*>	&getClientListChannel(const std::string &name) const;
		const bool  					&getOp(const std::string &chanName) const;
		void							setOp(const std::string &chanName, bool op, bool del);

		bool	checkPass(const std::string &password) const;
		void	sendMessage(const std::string &msg) const;
		void	receiveMessages();
		bool	nickAlreadyInUse(std::string arg, int clientFd);
		void	joinChannel(const std::string &name);
		void	createChannel(const std::string &name);
		int		leaveChannel(const std::string &name);
		bool	channelNameInUse(const std::string &name);
		bool	channelIsInviteOnly(const std::string &name);
		bool	channelIsFull(const std::string &name);
		bool	checkChannelPassword(const std::string &name, const std::string &pass);
		int		kickFromChannel(const std::string &chanName, const std::string &nickToKick, const std::string &msg);
		void	sendNameReply(const std::string &chanName);
    void sendToChannel(const std::string &message, int senderFd, const std::string &chanName);
  
	private:
		IRCServer					*_server;
		int							_fd;
		bool						_connected;
		bool						_sendWelcom;
		std::string					_nick;
		std::vector<std::string>	_user;
		std::map<std::string, bool>	_op;
};

#endif