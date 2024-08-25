#include "IRCServer.hpp"
#include "IRCClient.hpp"

IRCClient::IRCClient(int fd, IRCServer *server) : _server(server), _fd(fd)
{
  _connected = false;
  _sendWelcom = false;
}

IRCClient::~IRCClient() {}

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

const std::string &IRCClient::getUser() const
{
  return _user;
}

int IRCClient::getFd() const
{
  return _fd;
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

void IRCClient::setUser(std::string user)
{
  _user = user;
}

void IRCClient::sendMessage(const std::string &msg) const
{
  std::cout << "server ----> client : " << msg << std::endl;
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
      std::cout << "client ----> server : " << message << std::endl;
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
