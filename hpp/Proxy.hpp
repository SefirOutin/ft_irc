#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
#include <irc.hpp>
#include <string.h>
#include <vector>
#include <poll.h>
#include <errno.h>
#include <sys/ioctl.h>

class Proxy
{
  public:
    Proxy(int port, const char *destIp, const int destPort);

    ~Proxy();

    int startProxy();
    int run();
    int acceptConnections();
    void forwardData(size_t index);
    // void handleServerMessage(size_t index);
    // void removeConnection(int fd);
    
    // int connectToClient();
    // std::string listensMsg();
    // int sendMsg(const char *buffer);

  private:
    int _proxySockFd, _serverSockFd;
    const char *_destIp;
    sockaddr_in _proxySockAddr, _serverSockAddr;
    std::vector<struct pollfd> connections;
    std::map<int, int> client_to_server;
    std::map<int, int> server_to_client;
};