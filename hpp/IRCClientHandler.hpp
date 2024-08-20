#ifndef IRCCLIENTHANDLER_HPP
#define IRCCLIENTHANDLER_HPP

#include "IRCServer.hpp"

class IRCCommandParser;

class IRCClientHandler
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