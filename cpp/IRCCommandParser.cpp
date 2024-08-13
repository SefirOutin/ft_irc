#include "IRCCommandParser.hpp"
#include "Cmd.hpp"

IRCCommandParser::IRCCommandParser()
{
  registerCommand(new NickCommand());
  registerCommand(new UserCommand());
  registerCommand(new PassCommand());
}

IRCCommandParser::~IRCCommandParser()
{
  for (std::map<std::string, IRCCommandHandler *>::iterator it = commandHandlers.begin();
       it != commandHandlers.end(); ++it)
  {
    delete it->second;
  }
}

void IRCCommandParser::parseCommand(const std::string &command, IRCClientHandler &client)
{
  std::istringstream iss(command);
  std::string commandName;
  iss >> commandName;

  std::string params;
  std::getline(iss, params);

  std::map<std::string, IRCCommandHandler *>::iterator it = commandHandlers.find(commandName);
  if (it != commandHandlers.end())
  {
    it->second->execute(params, client);
  }
  else
  {
    client.sendMessage("421 " + commandName + " :Unknown command");
  }
}

void IRCCommandParser::registerCommand(IRCCommandHandler *handler)
{
  commandHandlers[handler->getCommandName()] = handler;
}
