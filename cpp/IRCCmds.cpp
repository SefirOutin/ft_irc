#include "IRCCmds.hpp"

void NickCommand::execute(const std::string &params, IRCClientHandler &client)
{
	std::cout << params << std::endl;
	if (client.checkPass())
		client.sendMessage(": 462  :You may not reregister !\r\n");
	else if (params.empty())
	{
		client.sendMessage(": 461 * :Not enough parameters.\r\n");
	}
	else if (params.compare())
	{
		close(clientFd);
		clients->erase(clientFd);
		client.
	}
	else
		(*clients)[clientFd].setPass(true);
}
std::string NickCommand::getCommandName() const
{
	return "NICK";
}

void PassCommand::execute(const std::string &params, IRCClientHandler &client)
{
	std::cout << params << std::endl;
	(void)params;
	(void)client;
}
std::string PassCommand::getCommandName() const
{
	return "PASS";
}

void UserCommand::execute(const std::string &params, IRCClientHandler &client)
{
	std::cout << params << std::endl;

	(void)params;
	(void)client;
}
std::string UserCommand::getCommandName() const
{
	return "USER";
}
