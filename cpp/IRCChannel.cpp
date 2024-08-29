#include "IRCChannel.hpp"


IRCChannel::IRCChannel(const std::string &name, IRCClient &clientOp):
	_name(name)
{
	
	_inviteOnly = false;
	_userLimit = -1;
	_nbUser = 0;
	newConnection(clientOp);
	newOperator(clientOp);
}

IRCChannel::~IRCChannel()
{
}

int	IRCChannel::getNbUser() const
{
	return (_nbUser);
}

bool	IRCChannel::getInviteOnly() const
{
	return (_inviteOnly);
}

const std::map<int, IRCClient *> IRCChannel::getListClientChannel() const
{
	return (_clients);
}

void	IRCChannel::newConnection(IRCClient &client)
{
	_clients.insert(std::pair<int, IRCClient *>(client.getFd(), &client));
	_nbUser++;
}

void	IRCChannel::newOperator(IRCClient &client)
{
	_operators.insert(std::pair<int, IRCClient *>(client.getFd(), &client));
}
void	IRCChannel::removeUser(int clientFd)
{
	std::map<int, IRCClient *>::iterator	it = _clients.find(clientFd);
	if (it != _clients.end())
	{
		_clients.erase(clientFd);
		_nbUser--;

	}
}

void	IRCChannel::sendToChannel(const std::string &message, int senderFd)
{
	std::map<int, IRCClient *>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->first != senderFd)
			it->second->sendMessage(message);
	}
}