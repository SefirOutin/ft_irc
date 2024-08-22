#ifndef IRCCLIENTHANDLER_HPP
#define IRCCLIENTHANDLER_HPP

#include "IRCServer.hpp"
#include "ft_irc.hpp"

class IRCClientHandler
{
	public:
		IRCClientHandler() {}
		IRCClientHandler(int fd);
		~IRCClientHandler();

		const std::string	getNick() const;
		void 				setNick(std::string nick);
		const std::string	getUser() const;
		void 				setUser(std::string user);
		int 				getFd() const;
		bool 				checkPass();
		void 				setPass(bool status);

		void sendMessage(const std::string &msg);

	private:
		int 		_fd;
		std::string	_nick;
		std::string	_user;
		bool		_pass;
};

#endif