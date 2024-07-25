#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
class Server
{

public:
  Server(int port);
  ~Server();

  int startServer();
  int listensMsg();

private:
  int _port, _sockFd;
  int _connection;
};