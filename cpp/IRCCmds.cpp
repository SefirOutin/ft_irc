#include "IRCCmds.hpp"

bool verify_string_format(const std::string &input_string)
{
	for (size_t i = 0; i < input_string.size(); i++)
	{
		char c = input_string[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || std::string("-[]\\`^{}").find(c) != std::string::npos))
			return (false);
	}
	return (true);
}

void NickCommand::execute(const std::string &params, IRCClient &client)
{
	if (params.empty())
		client.sendMessage(": 431 * :No nickname given.\r\n");
	else if (client.nickAlreadyInUse(params, client.getFd()))
		client.sendMessage(": 433 " + params + " :" + params + " is already in use\r\n");
	else if (!client.isConnected())
		client.sendMessage(": 451 * :You have not registered!\r\n");
	else if (!verify_string_format(params))
		client.sendMessage(": 432 * :Bad format on nickname\r\n");
	else if (params != client.getNick())
	{
		if (!client.getNick().empty())
			client.sendMessage(":" + client.getNick() + " NICK " + params + "\r\n");
		client.setNick(params);
	}
}

void PassCommand::execute(const std::string &params, IRCClient &client)
{
	if (client.isConnected())
		client.sendMessage(": 462  :You may not reregister !\r\n");
	else if (params.empty())
		client.sendMessage(": 461 * :Not enough parameters.\r\n");
	else if (!client.checkPass(params))
		close(client.getFd());
	else
		client.setConnected(true);
}

void UserCommand::execute(const std::string &params, IRCClient &client)
{
	if (client.isConnected() && !client.getNick().empty())
	{
		client.setUser(params);
		client.sendMessage(": 001 " + client.getNick() + " :Welcome to the IRC server!\r\n");
	}
}
