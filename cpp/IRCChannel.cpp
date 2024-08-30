#include "IRCChannel.hpp"


IRCChannel::IRCChannel(const std::string &name, IRCClient &clientOp):
	_name(name)
{
	
	_inviteOnly = false;
	_userLimit = -1;
	_nbUser = 0;
	newConnection(clientOp);
}

IRCChannel::~IRCChannel()
{
}

const int	&IRCChannel::getNbUser() const
{
	return (_nbUser);
}

const bool	&IRCChannel::getInviteOnly() const
{
	return (_inviteOnly);
}

void	IRCChannel::setInviteOnly(bool invite)
{
	_inviteOnly = invite;
}

const int	&IRCChannel::getUserLimit() const
{
	return (_userLimit);
}

void	IRCChannel::setUserLimit(int limit)
{
	_userLimit = limit;
}

const std::string	&IRCChannel::getPassword() const
{
	return (_password);
}

void	IRCChannel::setPassword(const std::string &password)
{
	_password = password;
}

const std::string	&IRCChannel::getTopic() const
{
	return (_topic);
}

void	IRCChannel::setTopic(const std::string &topic)
{
	_topic = topic;
}


const std::map<int, IRCClient *> &IRCChannel::getClientListChannel() const
{
	return (_clients);
}

const int	&IRCChannel::getClientFdInChannel(const std::string &nick) const
{
	std::map<int, IRCClient *>::const_iterator	it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNick() == nick)
			break;
	}
	// if (it == _clients.end())
	// 	return (-1);
	return (it->second->getFd());
}

void	IRCChannel::newConnection(IRCClient &client)
{
	_clients.insert(std::pair<int, IRCClient *>(client.getFd(), &client));
	_nbUser++;
}

void	IRCChannel::removeUser(int clientFd)
{
	std::map<int, IRCClient *>::iterator	it = _clients.find(clientFd);
	if (it == _clients.end())
		return ;
	_clients.erase(clientFd);
	_nbUser--;
}

void	IRCChannel::sendToChannel(const std::string &message, int senderFd)
{
	std::map<int, IRCClient *>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->first != senderFd)
			it->second->sendMessage(message + "\r\n");
	}
}