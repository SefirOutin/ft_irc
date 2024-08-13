#ifndef IRCCOMMANDPARSER
#define IRCCOMMANDPARSER
#include "IRCCommandHandler.hpp"
#include "IRCClientHandler.hpp"
#include <iostream>
#include <map>
#include <sstream>

class IRCCommandParser
{
public:
  IRCCommandParser();
  ~IRCCommandParser();
  void parseCommand(const std::string &command, IRCClientHandler &client);

private:
  std::map<std::string, IRCCommandHandler *> commandHandlers;
  void registerCommand(IRCCommandHandler *handler);
};

#endif