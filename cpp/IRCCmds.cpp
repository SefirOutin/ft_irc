#include "IRCCmds.hpp"

void NickCommand::execute(const std::string &params, IRCClientHandler &client)
{
  (void)params;
  (void)client;
}
std::string NickCommand::getCommandName() const
{
  return "NICK";
}