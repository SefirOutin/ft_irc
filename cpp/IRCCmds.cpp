#include "IRCCmds.hpp"

void NickCommand::execute(const std::string &params, IRCClient &client)
{
	if (params.empty())
	{
		std::string buff = ": 431 * :No nickname given.\r\n";
		send(client.getFd(), buff.c_str(), buff.size(), 0);
		// clients->erase(client.getFd());
	}
	else if (server.nickAlreadyInUse(params, client.getFd()))
	{
		std::cout << params << std::endl;

		std::string buff = ": 433 " + params + " :" + params + " is already in use\r\n";
		std::cout << buff;
		send(client.getFd(), buff.c_str(), buff.size(), 0);
		// clients->erase(client.getFd());
	}
	else if (!client.checkPass())
	{
		std::string buff = ": 451 * :You have not registered!\r\n";
		send(client.getFd(), buff.c_str(), buff.size(), 0);
		// clients->erase(client.getFd());
	}
	else if (!verify_string_format(params))
	{
		std::string buff = ": 432 * :Bad format on nickname\r\n";
		send(client.getFd(), buff.c_str(), buff.size(), 0);
		// clients->erase(client.getFd());
	}
	else if (params != client.getNick())
	{
		if (!client.getNick().empty())
		{
			std::string buff = ":" + client.getNick() + " NICK " + params + "\r\n";
			send(client.getFd(), buff.c_str(), buff.size(), 0);
		}
		client.setNick(params);
		std::cout << "ok frere" << std::endl;
	}
}

void PassCommand::execute(const std::string &params, IRCClient &client)
{
	std::cout << params << std::endl;

	if (client.isConnected())
	{
		std::string buff = ": 462  :You may not reregister !\r\n";
		send(client.getFd(), buff.c_str(), buff.size(), 0);
	}
	else if (params.empty())
	{
		std::string buff = ": 461 * :Not enough parameters.\r\n";
		send(client.getFd(), buff.c_str(), buff.size(), 0);

		// clients->erase(client.getFd());
	}
	else if (!client.checkPass(params))
	{
		close(client.getFd());
		// clients->erase(client.getFd());
	}
	else
		client.setConnected(true);
}

void UserCommand::execute(const std::string &params, IRCClient &client)
{
	client.setUser(params);
	if (client.isConnected() && !client.getNick().empty())
	{
		client.setUser(params);
		std::string buff = ": 001 " + client.getNick() + " :Welcome to the IRC server!\r\n";
		send(client.getFd(), buff.c_str(), buff.size(), 0);
	}
}
