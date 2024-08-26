#ifndef IRCCMDS_HPP
#define IRCCMDS_HPP

#include "IRCCommandHandler.hpp"
#include "IRCClient.hpp"

class NickCommand : public IRCCommandHandler
{
public:
	void execute(const std::string &params, IRCClient &client);
};

class PassCommand : public IRCCommandHandler
{
public:
	void execute(const std::string &params, IRCClient &client);
};

class UserCommand : public IRCCommandHandler
{
public:
	void execute(const std::string &params, IRCClient &client);
};

class JoinCommand : public IRCCommandHandler
{
public:
	void execute(const std::string &params, IRCClient &client);
};


#endif