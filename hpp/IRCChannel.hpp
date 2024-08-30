#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP

#include <utility>
#include "IRCServer.hpp"
// #include "IRCClient.hpp"

class IRCClient;

class IRCChannel
{
	public:
		IRCChannel(const std::string &name, IRCClient &clientOp);
		~IRCChannel();

		const int							&getNbUser() const;
		const bool							&getInviteOnly() const;
		void								setInviteOnly(bool invite);
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
	private:
		std::string					_name;
		std::map<int, IRCClient *>	_clients;
		std::string					_password;
		std::string					_topic;
		int							_userLimit;
		int							_nbUser;
		bool						_inviteOnly;

};



#endif