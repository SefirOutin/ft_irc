#include "Client.hpp"
#include <sstream>

Client::Client()
{
  _pass = false;
}

Client::~Client() {}

std::string Client::getNick()
{
  return _nick;
}

bool Client::checkPass()
{
  return _pass;
}

std::string Client::getUser()
{
  return _user;
}

void Client::setNick(std::string nick)
{
  _nick = nick;
}
void Client::setPass(bool pass)
{
  _pass = pass;
}
void Client::setUser(std::string user)
{
  _user = user;
}