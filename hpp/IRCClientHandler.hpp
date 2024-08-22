#ifndef IRCCLIENTHANDLER_HPP
#define IRCCLIENTHANDLER_HPP

#include "IRCServer.hpp"
#include "ft_irc.hpp"

class IRCClientHandler
{
public:
	IRCClientHandler(int fd);
	~IRCClientHandler();

	const std::string getNick() const;
	const std::string getUser() const;
	int getFd() const;

	bool checkPass();
	void setNick(std::string nick);
	void setUser(std::string user);
	void setPass(bool status);
	void sendMessage(const std::string &msg);

private:
	int _fd;
	std::string _nick;
	std::string _user;
	bool _pass;
};

#endif