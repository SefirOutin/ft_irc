#include "IRCCommandParser.hpp"
#include "IRCCmds.hpp"

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

  std::string line;
  std::stringstream buffer(command);

  int nLine = std::count(command.begin(), command.end(), '\n');
  while (nLine--)
  {
    if (std::getline(buffer, line))
    {
      std::string arg;
      size_t posLastCr = line.find("\r");
      size_t posFirstSpace = line.find(" ");
      std::string cmd = line.substr(0, posFirstSpace);
      if (posFirstSpace != arg.npos)
      {
        arg = line.substr(posFirstSpace + 1, posLastCr);
        arg.erase(std::remove(arg.begin(), arg.end(), '\r'), arg.end());
      }
      std::map<std::string, IRCCommandHandler *>::iterator it = commandHandlers.find(cmd);
      if (it != commandHandlers.end())
      {
        it->second->execute(arg, client);
      }
      else
      {
        client.sendMessage("421 " + cmd + " :Unknown command");
      }
      // std::cout << cmd << " " << arg << std::endl;
    }
  }
}

void IRCCommandParser::registerCommand(IRCCommandHandler *handler)
{
  commandHandlers[handler->getCommandName()] = handler;
}
