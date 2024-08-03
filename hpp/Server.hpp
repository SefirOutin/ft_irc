#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include <string>
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

class Server
{
  public:
    Server(int port, const std::string &password);

    ~Server();

    int startServer();
    int run();
    int acceptConnections();
    // void forwardData(size_t index);

  private:
    int 						_sockFd;
    sockaddr_in 				_sockAddr;
    std::vector<struct pollfd>	_fds;
	std::string					_password;
};