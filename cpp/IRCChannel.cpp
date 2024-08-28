#include "IRCChannel.hpp"

IRCChannel::IRCChannel(const std::string &name, const IRCClient &clientOp) : _name(name)
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

int IRCChannel::getNbUser() const
{
	return _nbUser;
}

void IRCChannel::newConnection(const IRCClient &client)
{
	IRCClient newClient(client);
	_clients.insert(std::pair<int, IRCClient>(client.getFd(), newClient));
	_nbUser++;
}

void IRCChannel::newOperator(const IRCClient &client)
{
	IRCClient newClient(client);
	_operators.insert(std::pair<int, IRCClient>(client.getFd(), newClient));
}
void IRCChannel::removeUser(int clientFd)
{
	std::map<int, IRCClient>::iterator it = _clients.find(clientFd);
	if (it != _clients.end())
		_clients.erase(clientFd);
	_nbUser--;
}

void IRCChannel::sendToChannel(const std::string &message)
{
	std::map<int, IRCClient>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		it->second.sendMessage(message);
	}
}
const std::map<int, IRCClient> IRCChannel::getListClientChannel() const
{
	return (_clients);
}