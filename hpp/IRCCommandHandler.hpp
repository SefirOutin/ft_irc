#ifndef IRCCOMMANDHANDLER_HPP
#define IRCCOMMANDHANDLER_HPP

#include <iostream>
#include "ft_irc.hpp"

class IRCClientHandler;
class IRCServer;

class IRCCommandHandler 
{
	public:
		// IRCCommandHandler(const std::string name): _name(name) {}
		virtual ~IRCCommandHandler(){};

		// const std::string getName() const {return (_name);}
		
		virtual void execute(IRCServer  &server, const std::string &params, IRCClientHandler &client) = 0;

	// private:
	// 	std::string	_name;
};

#endif