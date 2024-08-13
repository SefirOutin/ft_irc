#ifndef IRCCLIENTHANDLER
#define IRCCLIENTHANDLER

#include "IRCCommandParser.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

class IRCClientHandler
{
public:
  IRCClientHandler(int socket, IRCCommandParser &parser);
  void start();
  void stop();
  void sendMessage(const std::string &message);
  void setNick(const std::string &nick);
  std::string getNick() const;
  void setUser(const std::string &userInfo);

private:
  int _clientSocket;
  bool _connected;
  IRCCommandParser &_commandParser;
  std::string _nick;
  std::string _user;
  void receiveMessages();
};
#endif