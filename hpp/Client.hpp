#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
#include <irc.hpp>
#include <string.h>
class Client
{

public:
  Client(const char *ip, int port);

  ~Client();

  int connectToServer();
  std::string listensMsg();
  int sendMsg(const char *buffer);

private:
  int _port, _sockFd, _connection;
  const char *_ip;
  sockaddr_in _sockAddr;
};