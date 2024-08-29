#include "IRCServer.hpp"
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

const std::vector<std::string> IRCClient::getUser() const
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

bool IRCClient::getWelcom()
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

bool IRCClient::isConnected() const
{
  return (_connected);
}

int IRCClient::getFd() const
{
  return (_fd);
}

bool IRCClient::checkPass(const std::string &password) const
{
  if (password.compare(_server->getPass()))
    return (false);
  return (true);
}

std::string	IRCClient::getClientInfos()
{
	return (":" + _nick + "!~" + _user[0] + "@" + _user[2]);
}

IRCClient IRCClient::getClient(const std::string &nick)
{
	IRCClient	client(0, NULL);
  	std::map<int, IRCClient>::const_iterator it;
  	for (it = _server->getClients().begin(); it != _server->getClients().end(); ++it)
  	{
  	  if (it->second.getNick() == nick)
  	    client = it->second;
  	}
  	return (client);
}

const std::map<int, IRCClient *> IRCClient::getListClientChannel(const std::string &name) const
{
  return (_server->getChannels().find(name)->second.getListClientChannel());
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

bool  IRCClient::channelNameAlreadyInUse(const std::string &name)
{
  std::map<std::string, IRCChannel>::const_iterator it = _server->getChannels().find(name);
	if (it == _server->getChannels().end())
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
}

void	IRCClient::joinChannel(const std::string &name)
{
  _server->newConnectionToChannel(name, *this);
}

int  IRCClient::leaveChannel(const std::string &name)
{
	std::map<std::string, IRCChannel>::const_iterator itChannels= _server->getChannels().find(name);
	if (itChannels == _server->getChannels().end())
		return (1);
	std::map<int, IRCClient *>::const_iterator	itClientList = getListClientChannel(name).find(_fd);
	if (itClientList == getListClientChannel(name).end())
		return (2);
  	_server->removeClientFromChannel(name, *this);
	return (0);
}