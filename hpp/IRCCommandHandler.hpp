#ifndef IRCCOMMANDHANDLER_HPP
#define IRCCOMMANDHANDLER_HPP

#include <iostream>
#include "IRCClient.hpp"

class	IRCClient;

class	IRCCommandHandler
{
	public:
		virtual ~IRCCommandHandler() {};
		
		virtual void execute(const std::string &params, IRCClient &client) = 0;
};

#endif