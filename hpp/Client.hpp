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

class Client
{
private:
  std::string _nick;
  std::string _realName;
  std::string _userName;
  std::string _ip;
  std::string parseValue(const std::string &buff, const std::string &command);

public:
  Client(std::string buff);
  ~Client();
  std::string getNick();
  std::string getRealName();
  std::string getUserName();
  std::string getIp();
};
