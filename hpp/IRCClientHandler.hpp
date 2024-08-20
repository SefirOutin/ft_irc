#ifndef IRCCLIENTHANDLER
#define IRCCLIENTHANDLER

#include "IRCCommandParser.hpp"
#include "IRCServer.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

class IRCCommandParser;
class IRCClientHandler : public IRCServer
{
	public:
		IRCClientHandler(int socket, IRCCommandParser &parser);
		
		void setNick(const std::string &nick);
		std::string getNick() const;
		void setUser(const std::string &userInfo);
		void setPass(bool status);
		
		void start();
		void stop();
		bool checkPass();
		void sendMessage(const std::string &message);

	private:
		int _clientSocket;
		bool _connected;
		bool _pass;
		IRCCommandParser &_commandParser;
		std::string _nick;
		std::string _user;
		
		void receiveMessages();
};
#endif