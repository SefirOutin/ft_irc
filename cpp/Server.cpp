#include "Server.hpp"
#include "irc.hpp"

Server::Server(int port) : _port(port)
{
  this->_connection = 0;
}

Server::~Server()
{
  close(_sockFd);
  close(_connection);
}

int Server::startSocket()
{

  _sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sockFd < 0)
  {
    std::cerr << "socket error\n";
    return 1;
  }
  _sockAddr.sin_family = AF_INET;
  _sockAddr.sin_port = htons(_port);
  _sockAddr.sin_addr.s_addr = INADDR_ANY;
  if (bind(_sockFd, (struct sockaddr *)&_sockAddr, sizeof(_sockAddr)) < 0)
  {
    std::cerr << "bind error\n";
    return 1;
  }
  if (listen(_sockFd, SOMAXCONN) < 0)
  {
    std::cerr << "listen error\n";
    return 1;
  }
  std::cout << "Listens ...\n";
  return (0);
}

int Server::connectToClient()
{
  socklen_t addrLen;
  while (_connection <= 0)
  {
    addrLen = sizeof(_sockAddr);
    _connection = accept(_sockFd, (struct sockaddr *)&_sockAddr, &addrLen);
    if (_connection < 0)
    {
      std::cerr << "accept error\n";
      continue;
    }
    std::cout << "Connection made with " << inet_ntoa(_sockAddr.sin_addr) << "\n";
  }
  return 0;
}

int Server::sendMsg(const char *buffer)
{
  if (send(_connection, buffer, strlen(buffer), 0) < 0)
    std::cerr << "send error\n";
  return (0);
}

std::string Server::listensMsg()
{
  char buffer[256];

  int n = recv(_connection, buffer, 255, 0);
  if (n < 0)
  {
    std::cerr << "Error receiving message from client\n";
    close(_connection);
    return (NULL);
  }
  else if (n == 0)
  {
    std::cout << "Client disconnected\n";
    close(_connection);
    return (NULL);
  }
  buffer[n] = '\0';
  std::cout << "Receive from client >>>> " << buffer;
  return std::string(buffer);
}