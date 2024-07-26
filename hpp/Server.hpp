#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
#include <irc.hpp>
#include <string.h>
class Server
{

public:
  Server(int port);

  ~Server();

  int startSocket();
  int connectToClient();
  std::string listensMsg();
  int sendMsg(const char *buffer);

private:
  int _port, _sockFd, _connection;
  sockaddr_in _sockAddr;
};