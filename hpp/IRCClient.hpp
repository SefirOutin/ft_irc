#ifndef IRCCLIENT_HPP
#define IRCCLIENT_HPP

#include "IRCServer.hpp"
#include "ft_irc.hpp"

class Client
{
public:
	Client();
	~Client();

	std::string getNick();
	std::string getUser();
	bool checkPass();
	void setNick(std::string nick);
	void setUser(std::string user);
	void setPass(bool status);

private:
	std::string _nick;
	std::string _user;
	bool _pass;
};

#endif