#include "IRCCommandHandler.hpp"
#include "IRCClientHandler.hpp"
class NickCommand : public IRCCommandHandler
{
public:
  void execute(const std::string &param, IRCClientHandler &client);
  std::string getCommandName() const;
};

class PassCommand : public IRCCommandHandler
{
public:
  void execute(const std::string &param, IRCClientHandler &client);
  std::string getCommandName() const;
};

class UserCommand : public IRCCommandHandler
{
public:
  void execute(const std::string &param, IRCClientHandler &client);
  std::string getCommandName() const;
};