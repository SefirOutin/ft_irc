#ifndef IRCCOMMANDPARSER_HPP
#define IRCCOMMANDPARSER_HPP

#include <sstream>
#include <algorithm>

#include "IRCCommandHandler.hpp"
#include "IRCClientHandler.hpp"

class IRCCommandParser
{
	public:
		IRCCommandParser();
		~IRCCommandParser();
		
		void parseCommand(const std::string &command, IRCClientHandler &client);

	private:
		std::map<std::string, IRCCommandHandler *> _commandHandlers;
		
		void registerCommand(IRCCommandHandler *handler);
};

#endif