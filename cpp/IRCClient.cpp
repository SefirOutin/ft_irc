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
  return _nick;
}

bool IRCClient::checkPass(const std::string &password) const
{
  if (password.compare(_server->getPass()))
    return (false);
  return (true);
}

const std::vector<std::string> IRCClient::getUser() const
{
  return _user;
}

int IRCClient::getFd() const
{
  return _fd;
}

std::string	IRCClient::getClientInfos()
{
	return (":" + _nick + "!~" + _user[0] + "@" + _user[2]);
}

void IRCClient::setNick(std::string nick)
{
  _nick = nick;
}
void IRCClient::setConnected(bool status)
{
  _connected = status;
}

bool IRCClient::isConnected() const
{
  return (_connected);
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

bool IRCClient::getWelcom()
{
  return _sendWelcom;
}
void IRCClient::setWelcom(bool status)
{
  _sendWelcom = status;
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
  	std::map<std::string, IRCChannel>::iterator it = _server->getChannels().find(name);
	if (it == _server->getChannels().end())
		return (false);
	return (true);
}

void  IRCClient::createChannel(const std::string &name)
{
	IRCChannel	channel(name, *this);
	_server->getChannels().insert(std::pair<std::string, IRCChannel>(name, channel));
	// _server->getChannels()[name] = IRCChannel(name, *this);
}

void	IRCClient::joinChannel(const std::string &name)
{
	std::map<std::string, IRCChannel>::iterator it = _server->getChannels().find(name);
	// _server->getChannels()[name].newConnection(*this);
	it->second.newConnection(*this);
}

int  IRCClient::leaveChannel(const std::string &name)
{
	std::map<std::string, IRCChannel>::iterator it = _server->getChannels().find(name);
	if (it == _server->getChannels().end())
	{
		sendMessage(":??? 403 " + _nick + " " + name + " :No such channel\r\n");
		return (1);
	}
	it->second.removeUser(_fd);
	if (!it->second.getNbUser())
		_server->removeChannel(name);
	return (0);
}