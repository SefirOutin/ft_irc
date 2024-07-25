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

int Server::startServer()
{

  socklen_t addrLen;
  sockaddr_in sockAddr;

  _sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sockFd < 0)
  {
    std::cerr << "socket error\n";
    return 1;
  }

  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(_port);
  sockAddr.sin_addr.s_addr = INADDR_ANY; //?

  if (bind(_sockFd, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0)
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
  while (_connection <= 0)
  {
    addrLen = sizeof(sockAddr);
    _connection = accept(_sockFd, (struct sockaddr *)&sockAddr, &addrLen);
    if (_connection < 0)
    {
      std::cerr << "accept error\n";
      continue;
    }
    std::cout << "Connection made with " << inet_ntoa(sockAddr.sin_addr);
  }
  return 0;
}

int Server::listensMsg()
{
  char buffer[256];
  while (1)
  {
    int n = recv(_connection, buffer, 255, 0);
    if (n < 0)
    {
      std::cerr << "Error receiving message from client\n";
      close(_connection);
      return (1);
    }
    else if (n == 0)
    {
      std::cout << "Client disconnected\n";
      close(_connection);
      return (1);
    }
    buffer[n] = '\0';
    std::cout << buffer << "\n";
    if (send(_connection, ": 001 bilou : Welcome to the IRC server!\n", 42, 0) < 0)
    {
      std::cerr << "send error\n";
      return (1);
    }
  }
}