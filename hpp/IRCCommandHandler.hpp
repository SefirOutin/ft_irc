#ifndef IRCCOMMANDHANDLER
#define IRCCOMMANDHANDLER

#include <iostream>

class IRCClientHandler;
class IRCCommandHandler 
{
public:
  virtual ~IRCCommandHandler(){};
  virtual void execute(const std::string &params, IRCClientHandler &client) = 0;
  virtual std::string getCommandName() const = 0;
};

#endif