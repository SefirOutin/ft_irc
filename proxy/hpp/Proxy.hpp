#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <poll.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

class Proxy
{
  public:
    Proxy(int port, const char *destIp, const int destPort);

    ~Proxy();

    int startProxy();
    int run();
    int acceptConnections();
    void forwardData(size_t index);
    int connectToServer();

  private:
    int _proxySockFd, _serverSockFd;
    const char *_destIp;
    sockaddr_in _proxySockAddr, _serverSockAddr;
    std::vector<struct pollfd> _fds;
};