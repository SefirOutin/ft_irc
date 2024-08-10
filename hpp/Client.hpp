#pragma once

#include <string>
#include "Server.hpp"

class Client
{
public:
	Client();
	~Client();

	std::string	getNick();
	std::string	getPass();
	std::string	getUser();
	void		setNick(std::string nick);
	void		setPass(std::string pass);
	void		setUser(std::string user);

private:
	std::string _nick;
	std::string _pass;
	std::string _user;
};
