#ifndef IRCCMDS_HPP
#define IRCCMDS_HPP

#include "IRCCommandHandler.hpp"
#include "IRCClientHandler.hpp"
#include "IRCServer.hpp"

class IRCServer;

class NickCommand : public IRCCommandHandler
{
  	public:
		void execute(IRCServer  &server, const std::string &params, IRCClientHandler &client);
};

class PassCommand : public IRCCommandHandler
{
	public:
		void execute(IRCServer  &server, const std::string &params, IRCClientHandler &client);
};

class UserCommand : public IRCCommandHandler
{
	public:
		void execute(IRCServer  &server, const std::string &params, IRCClientHandler &client);
};

#endif