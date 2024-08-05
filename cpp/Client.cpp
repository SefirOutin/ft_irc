#include "Client.hpp"
#include <sstream>

std::string Client::parseValue(const std::string &buff, const std::string &command)
{
  std::istringstream stream(buff);
  std::string line;
  while (std::getline(stream, line))
  {
    std::cout << line << std::endl;
  }
  return "";
}

Client::Client(std::string buff)
{
  _nick = parseValue(buff, "NICK");
  _userName = parseValue(buff, "USER");
  size_t colonPos = _userName.find(" :");
  if (colonPos != std::string::npos)
  {
    _realName = _userName.substr(colonPos + 2);
    _userName = _userName.substr(0, colonPos);
    
  }
  else
  {
    _realName = "";
  }

  std::istringstream userStream(_userName);
  userStream >> _userName >> _userName >> _ip;
}

// Destructor
Client::~Client() {}

// Getters
std::string Client::getNick()
{
  return _nick;
}

std::string Client::getRealName()
{
  return _realName;
}

std::string Client::getUserName()
{
  return _userName;
}

std::string Client::getIp()
{
  return _ip;
}
