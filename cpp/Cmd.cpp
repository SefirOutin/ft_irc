#include "Cmd.hpp"
void NickCommand::execute(const std::string &params, IRCClientHandler &client)
{
  
}
std::string NickCommand::getCommandName() const
{
  return "NICK";
}