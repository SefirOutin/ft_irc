#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP

#include <utility>
#include "IRCServer.hpp"
// #include "IRCClient.hpp"

#define MODE_I 0
#define MODE_T 1
#define MODE_K 2
#define MODE_O 3
#define MODE_L 4

class IRCClient;

class IRCChannel
{
	public:
		IRCChannel(const std::string &name, IRCClient &clientOp);
		~IRCChannel();

		const int							&getNbUser() const;
		const int							&getUserLimit() const;
		void								setUserLimit(int limit);
		const std::string					&getPassword() const;
		void								setPassword(const std::string &password);
		const std::string					&getTopic() const;
		void								setTopic(const std::string &topic);
		const std::map<int, IRCClient *>	&getClientListChannel() const;
		const int							&getClientFdInChannel(const std::string &name) const;

		void	newConnection(IRCClient &client);
		void	removeUser(int clientFd);
		void	sendToChannel(const std::string &message, int senderFd);
		void	setMode(const std::string &mode);
		bool	inMode(const std::string &mode);
		void	whiteList(const int &fd);
		bool	isWhiteListed(const int &fd);
		void	changeOpe(const std::string &nick, bool op);
		void	setKey(const std::string &key);
		void	sendToChannelMode(const std::string &message);

	private:
		std::string					_name;
		std::map<int, IRCClient *>	_clients;
		std::string					_password;
		std::string					_topic;
		int							_userLimit;
		int							_nbUser;
		bool						_modes[5];
		std::map<int, bool>			_whiteList;

};



#endif