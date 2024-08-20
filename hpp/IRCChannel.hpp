#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP

#include "IRCServer.hpp"

class Channel
{
	public:
		Channel(/* args */);
		~Channel();

	private:
		std::string						_name;
		// std::vector<IRCClientHandler>	_clients;
		// std::map<int, IRCClientHandler>	_clients;
};

#endif