#ifndef IRCCOMMANDHANDLER
#define IRCCOMMANDHANDLER

#include <iostream>
class IRCClientHandler; // Forward declaration
class IRCCommandHandler
{
public:
  virtual ~IRCCommandHandler() = default;

  // Méthode purement virtuelle à implémenter par les classes dérivées
  virtual void execute(const std::string &params, IRCClientHandler &client) = 0;

  // Optionnel : une méthode pour obtenir le nom de la commande
  virtual std::string getCommandName() const = 0;
};

#endif