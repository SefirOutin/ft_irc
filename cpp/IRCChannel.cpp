#include "IRCChannel.hpp"

IRCChannel::IRCChannel(const std::string &name, IRCClient &clientOp) : _name(name)
{
	_userLimit = -1;
	_nbUser = 0;
	_modes[MODE_I] = false;
	_modes[MODE_T] = false;
	_modes[MODE_K] = false;
	_modes[MODE_O] = false;
	_modes[MODE_L] = false;
	newConnection(clientOp);
}

IRCChannel::~IRCChannel()
{
}

const int	&IRCChannel::getNbUser() const
{
	return (_nbUser);
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

void IRCChannel::sendToChannel(const std::string &message, int senderFd)
{
	std::map<int, IRCClient *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->first != senderFd)
			it->second->sendMessage(message + "\r\n");
	}
}

void IRCChannel::sendToChannelMode(const std::string &message)
{
	std::map<int, IRCClient *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
		it->second->sendMessage(message + "\r\n");
}


void	IRCChannel::setMode(const std::string &mode)
{
	if (mode == "+i")
		_modes[MODE_I] = true;
	else if (mode == "-i")
		_modes[MODE_I] = false;
	else if (mode == "+t")
		_modes[MODE_T] = true;
	else if (mode == "-t")
		_modes[MODE_T] = false;
	else if (mode == "+k")
		_modes[MODE_K] = true;
	else if (mode == "-k")
		_modes[MODE_K] = false;
	else if (mode == "+o")
		_modes[MODE_O] = true;
	else if (mode == "-o")
		_modes[MODE_O] = false;
	else if (mode == "+l")
		_modes[MODE_L] = true;
	else if (mode == "-l")
		_modes[MODE_L] = false;
}

bool	IRCChannel::inMode(const std::string &mode)
{
	if (mode == "i")
		return (_modes[MODE_I]);
	else if (mode == "t")
		return (_modes[MODE_T]);
	else if (mode == "k")
		return (_modes[MODE_K]);
	else if (mode == "o")
		return (_modes[MODE_O]);
	else if (mode == "l")
		return (_modes[MODE_L]);
	return (false);
}

void	IRCChannel::whiteList(const int &fd)
{
	_whiteList.insert(std::pair<int, bool>(fd, true));
}

bool	IRCChannel::isWhiteListed(const int &fd)
{
	std::map<int, bool>::iterator it = _whiteList.find(fd);
	if (it != _whiteList.end())
		return (true);
	return (false);
}

void	IRCChannel::changeOpe(const std::string &nick, bool op)
{
	std::map<int, IRCClient *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNick() == nick)
			break;
	}
	if (it == _clients.end())
		return ;
	it->second->setOp(_name, op, false);
}

void	IRCChannel::setKey(const std::string &key)
{
	_password = key;
}
