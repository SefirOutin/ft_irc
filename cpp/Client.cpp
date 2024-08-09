#include "Client.hpp"
#include <sstream>

Client::Client()
{
}

Client::~Client() {}

std::string Client::getNick()
{
  return _nick;
}

std::string Client::getPass()
{
  return _pass;
}

std::string Client::getUser()
{
  return _user;
}


int	Client::getFd()
{
	return _fd;
}

void Client::setNick(std::string nick)
{
  _nick = nick;
}
void Client::setPass(std::string pass)
{
  _pass = pass;
}
void Client::setUser(std::string user)
{
  _user = user;
}

void	Client::setFd(int fd)
{
	_fd = fd;
}