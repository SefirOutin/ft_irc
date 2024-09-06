#include "IRCClient.hpp"

IRCClient::IRCClient(int fd, IRCServer *server) : _server(server), _fd(fd)
{
  	_connected = false;
  	_sendWelcom = false;
}

IRCClient::IRCClient(const IRCClient &other)
{
  *this = other;
}

IRCClient::~IRCClient() {}

IRCClient &IRCClient::operator=(const IRCClient &other)
{
  if (this == &other)
    return (*this);
  _server = other._server;
  _fd = other._fd;
  _connected = other._connected;
  _sendWelcom = other._sendWelcom;
  _nick = other._nick;
  _user = other._user;
  return (*this);
}

const std::string &IRCClient::getNick() const
{
  	return (_nick);
}

void IRCClient::setNick(std::string nick)
{
  	_nick = nick;
}

const std::vector<std::string> &IRCClient::getUser() const
{
  	return (_user);
}

void IRCClient::setUser(const std::string &user)
{
  	std::istringstream arg(user);
  	_user.resize(4);
  	std::string firstName;
  	std::string lastName;
  	size_t i = 0;
  	while (i < 5)
  	{
  	  	if (i == 3)
  	  	{
  	  	  	arg >> firstName;
  	  	  	arg >> lastName;
  	  	  	if (firstName[0] == ':')
  	  	    	firstName.erase(0, 1);
  	  	  	_user[i] = firstName + " " + lastName;
  	  	}
  	  	else
  	  	  	arg >> _user[i];
  	  	++i;
  	}
}

const bool &IRCClient::getWelcom() const
{
  	return (_sendWelcom);
}
void IRCClient::setWelcom(bool status)
{
  	_sendWelcom = status;
}
void IRCClient::setConnected(bool status)
{
  	_connected = status;
}

const bool &IRCClient::isConnected() const
{
  	return (_connected);
}

const int &IRCClient::getFd() const
{
  	return (_fd);
}

bool IRCClient::checkPass(const std::string &password) const
{
  	if (password.compare(_server->getPass()))
  	  	return (false);
  	return (true);
}
std::string	IRCClient::getClientInfos() const
{
  return (":" + _nick + "!~" + _user[0] + "@" + _user[2]);
}

const IRCClient &IRCClient::getClient(const std::string &nick) const
{
  IRCClient client(0, NULL);
  std::map<int, IRCClient>::const_iterator it;
  for (it = _server->getClients().begin(); it != _server->getClients().end(); ++it)
  {
    if (it->second.getNick() == nick)
      break;
  }
  return (it->second);
}

void	IRCClient::setOp(const std::string &chanName, bool op, bool del)
{
	if (del)
		_op.erase(chanName);
	else
		_op[chanName] = op;
	
}

void IRCClient::sendMessage(const std::string &msg) const
{
  //   std::cout << "server ----> client : " << msg << std::endl;
  send(_fd, msg.c_str(), msg.length(), 0);

}

void IRCClient::receiveMessages()
{
  char buffer[512];
  {
    int bytesReceived = recv(_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0)
    {
      buffer[bytesReceived] = '\0';
      std::string message(buffer);
      //   std::cout << "client ----> server : " << message << std::endl;
      _server->parseCmds(message, *this);
    }
    else
    {
      _server->closeConnection(_fd);
    }
  }
}

bool IRCClient::nickAlreadyInUse(std::string arg, int clientFd)
{
  	std::map<int, IRCClient>::const_iterator it;
  	for (it = _server->getClients().begin(); it != _server->getClients().end(); it++)
  	{
  	  if (!it->second.getNick().compare(arg) && it->first != clientFd)
  	    return (true);
  	}
  	return (false);
}

bool  IRCClient::channelNameInUse(const std::string &name)
{
  	if (_server->getChannels().find(name) == _server->getChannels().end())
		return (false);
	return (true);
}

bool  IRCClient::channelIsInviteOnly(const std::string &name)
{
  	std::map<std::string, IRCChannel>::const_iterator it = _server->getChannels().find(name);
	if (it->second.getInviteOnly())
    	return (true);
  	return (false);
}

bool	IRCClient::channelIsFull(const std::string &name)
{
	std::map<std::string, IRCChannel>::const_iterator it = _server->getChannels().find(name);
	if (it->second.getUserLimit() > -1)
	{
		if (it->second.getNbUser() == it->second.getUserLimit())
			return (true);
	}
	return (false);
}

bool	IRCClient::checkChannelPassword(const std::string &name, const std::string &pass)
{
	std::map<std::string, IRCChannel>::const_iterator it = _server->getChannels().find(name);
	if (!it->second.getPassword().compare(pass))
		return (true);
	return (false);
}

void  IRCClient::createChannel(const std::string &name)
{
  	_server->newChannel(name, *this);
	_op[name] = true;
  	// _op.insert(std::pair<std::string, bool>(name, true));
	// std::cout << _op[name] << "\n";
}

void IRCClient::joinChannel(const std::string &name)
{
	_server->newConnectionToChannel(name, *this);
	_op[name] = false;
	// _op.insert(std::pair<std::string, bool>(name, false));
	std::cout << _op[name] << "\n";
}

int IRCClient::leaveChannel(const std::string &name)
{
  std::map<std::string, IRCChannel>::const_iterator itChannels= _server->getChannels().find(name);
	if (itChannels == _server->getChannels().end())
		return (1);
	std::map<int, IRCClient *>::const_iterator	itClientList = getClientListChannel(name).find(_fd);
	if (itClientList == getClientListChannel(name).end())
		return (2);
	_server->removeClientFromChannel(name, _fd);
	return (0);
}

int	IRCClient::kickFromChannel(const std::string &chanName, const std::string &nickToKick, const std::string &msg)
{
	std::map<int, IRCClient *>	list = getClientListChannel(chanName);
	if (list.find(_fd) == list.end())
		return (1);
	if (!_op[chanName])
		return (2);
	if (!channelNameInUse(chanName))
		return (3);
	std::map<int, IRCClient *>::const_iterator	it;
	for (it = list.begin(); it != list.end(); it++)
	{
		if (it->second->getNick() == nickToKick)
			break;
	}
	if (it == list.end())
		return (4);
	_server->removeClientFromChannel(chanName, it->second->getFd());
	it->second->sendMessage(":" + _nick + " KICK " + chanName + " " + it->second->getNick() + " :" + msg + "\r\n");

	return (0);
}

void	IRCClient::sendNameReply(const std::string &chanName)
{
	std::string	reply;
	std::map<int, IRCClient *>	list = getClientListChannel(chanName);
	
	reply = ":353 " + _nick + " = " + chanName + " :";
	std::map<int, IRCClient *>::const_iterator	it;
	for (it = list.begin(); it != list.end(); it++)
	{
		if (it->second->getOp(chanName))
			reply += "@";
		else
			reply += "+";
		reply += it->second->getNick() + " ";
	}
	reply += "\r\n";
	sendMessage(reply);
	sendMessage(": 366 " + _nick + " " + chanName + " :END of /NAMES list\r\n");
}

void IRCClient::sendToChannel(const std::string &message, int senderFd, const std::string &chanName)
{ 
  _server->sendToChannel(message, senderFd, chanName);
}
