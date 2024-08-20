#ifndef IRCCMDS_HPP
#define IRCCMDS_HPP

#include "IRCCommandHandler.hpp"
#include "IRCClientHandler.hpp"

class NickCommand : public IRCCommandHandler
{
  	public:
		std::string getCommandName() const;
		
		void execute(const std::string &param, IRCClientHandler &client);
};

class PassCommand : public IRCCommandHandler
{
	public:
		std::string getCommandName() const;

		void execute(const std::string &param, IRCClientHandler &client);
};

class UserCommand : public IRCCommandHandler
{
	public:
		std::string getCommandName() const;
		
		void execute(const std::string &param, IRCClientHandler &client);
};

#endif