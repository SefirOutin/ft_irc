#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP

#include <utility>
#include "IRCServer.hpp"
// #include "IRCClient.hpp"

class IRCClient;

class IRCChannel
{
<<<<<<< Updated upstream
public:
	IRCChannel(const std::string &name, const IRCClient &clientOp);
	~IRCChannel();

	int getNbUser() const;
	const std::map<int, IRCClient> getListClientChannel() const;
	void newConnection(const IRCClient &client);
	void newOperator(const IRCClient &client);
	void removeUser(int clientFd);
	void sendToChannel(const std::string &message);
=======
	public:
		IRCChannel(const std::string &name, IRCClient &clientOp);
		~IRCChannel();

		int	getNbUser() const;

		void	newConnection(IRCClient &client);
		void	newOperator(IRCClient &client);
		void	removeUser(int clientFd);
		void	sendToChannel(const std::string &message, int senderFd);
	private:
		std::string					_name;
		std::map<int, IRCClient *>	_clients;
		std::map<int, IRCClient *>	_operators;
		std::string					_password;
		int							_userLimit;
		int							_nbUser;
		bool						_inviteOnly;
>>>>>>> Stashed changes

private:
	std::string _name;
	std::map<int, IRCClient> _clients;
	std::map<int, IRCClient> _operators;
	std::string _password;
	int _userLimit;
	int _nbUser;
	bool _inviteOnly;
};

#endif