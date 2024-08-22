#ifndef IRCCOMMANDHANDLER_HPP
#define IRCCOMMANDHANDLER_HPP

#include <iostream>
#include "ft_irc.hpp"

class IRCClientHandler;
class IRCServer;

class IRCCommandHandler 
{
	public:
		virtual ~IRCCommandHandler(){};

		virtual void execute(IRCServer  &server, const std::string &params, IRCClientHandler &client) = 0;

};

#endif