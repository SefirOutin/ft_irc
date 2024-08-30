#include "IRCCmds.hpp"

bool	verify_string_format(const std::string &input_string)
{
	for (size_t i = 0; i < input_string.size(); i++)
	{
		char c = input_string[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || std::string("_-[]\\`^{}").find(c) != std::string::npos))
			return (false);
		else if (i >= 9)
			return (false);
	}
	return (true);
}

bool	checkArgUser(const std::string &params)
{
	std::istringstream arg(params);
	std::string word;
	int i = 0;
	while (arg >> word)
		i++;
	if (i == 4)
		return (true);
	return (false);
}

void	NickCommand::execute(const std::string &params, IRCClient &client)
{
	// std::cout << params << "\n";
	if (params.empty())
		client.sendMessage(ERR_NONICKNAMEGIVEN);
	else if (!client.isConnected())
		client.sendMessage(ERR_NOTREGISTERED);
	else if (!verify_string_format(params))
		client.sendMessage(ERR_ERRONEUSNICKNAME(params));
	else if (client.nickAlreadyInUse(params, client.getFd()))
	{
		client.sendMessage(ERR_NICKNAMEINUSE(params));
		if (client.getNick().empty())
			client.setNick(params);
	}
	else if (params != client.getNick())
	{
		if (!client.getNick().empty())
		{
			if (!client.getWelcom() && !client.getUser().empty())
			{
				client.sendMessage(": 001 " + client.getNick() + " :Welcome to the IRC server!\r\n");
				client.setWelcom(true);
			}
			if (client.getWelcom())
				client.sendMessage(":" + client.getNick() + " NICK " + params + "\r\n");
		}
		client.setNick(params);
	}
}

void	PassCommand::execute(const std::string &params, IRCClient &client)
{
	if (client.isConnected())
		client.sendMessage(ERR_ALREADYREGISTRED);
	else if (params.empty())
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "PASS"));
	else if (!client.checkPass(params))
		client.setConnected(false);
	else
		client.setConnected(true);
}

void	UserCommand::execute(const std::string &params, IRCClient &client)
{
	if (client.isConnected() && !client.getNick().empty())
	{
		client.setUser(params);
		if (client.nickAlreadyInUse(client.getNick(), client.getFd()))
		{
			client.sendMessage(ERR_NICKNAMEINUSE(client.getNick()));
			client.setNick(client.getNick());
		}
		else
		{
			client.sendMessage(": 001 " + client.getNick() + " :Welcome to the IRC server!\r\n");
			client.setWelcom(true);
		}
	}
}

void PingCommand::execute(const std::string &params, IRCClient &client)
{
	(void)params;
	client.sendMessage("PONG\r\n");
}

void	CapCommand::execute(const std::string &params, IRCClient &client)
{
	(void)params;
	(void)client;
}

void	PrivmsgCommand::execute(const std::string &params, IRCClient &client)
{
	if (!client.getWelcom())
		return;
	std::string split, nickOrChanToSend, msgToSend;
	std::istringstream arg(params);
	for (size_t i = 0; std::getline(arg, split, ':'); i++)
		(i == 0) ? nickOrChanToSend = split.erase(split.length() - 1, split.length()) : msgToSend = split;
	if (nickOrChanToSend[0] == '#')
	{
		std::string name(nickOrChanToSend);
		nickOrChanToSend.erase(0, 1);
		if (client.channelNameInUse(nickOrChanToSend))
		{
			std::map<int, IRCClient *>::const_iterator it;
			for (it = client.getClientListChannel(nickOrChanToSend).begin(); it != client.getClientListChannel(nickOrChanToSend).end(); it++)
			{
				if (it->second->getNick() != client.getNick())
					it->second->sendMessage(client.getClientInfos() + " PRIVMSG " + name + " :" + msgToSend + "\r\n");
			}
		}
	}
	else if (client.nickAlreadyInUse(nickOrChanToSend, client.getFd()))
		client.getClient(nickOrChanToSend).sendMessage(client.getClientInfos() + " PRIVMSG " + nickOrChanToSend + " :" + msgToSend + "\r\n");
	// PRIVMSG bilel : ca va
	// : bi!~bmoudach@localhost PRIVMSG bilel : ca va
}
void 	JoinCommand::execute(const std::string &params, IRCClient &client)
{
	if (!params.size())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "JOIN"));
		return ;
	}
	if (params[0] != '#' && params[0] != '&')
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), params));
		return ;
	}
	// std::string name = params.substr(1, params.length() - 1);
	if (client.channelNameInUse(params))
	{
		if (client.channelIsInviteOnly(params))
		{
			client.sendMessage(ERR_INVITEONLYCHANNEL(client.getNick(), params));
			return ;
		}
		if (client.channelIsFull(params))
		{
			client.sendMessage(ERR_CHANNELISFULL(client.getNick(), params));
			return ;
		}
		client.joinChannel(params);
	}
	else
		client.createChannel(params);
	client.sendMessage(client.getClientInfos() + " JOIN " + params + "\r\n");
}

void	PartCommand::execute(const std::string &params, IRCClient &client)
{
	if (!params.size())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "PART"));
		return ;
	}
	// std::string name = params.substr(1, params.length() - 1);
	switch(client.leaveChannel(params))
	{
		case 0:
			client.sendMessage(client.getClientInfos() + " PART " + params + "\r\n");
			break;
		case 1:
			client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), params));
			break;
		case 2:
			client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), params));
			break;
	}
}

void	KickCommand::execute(const std::string &params, IRCClient &client)
{
	std::istringstream	sstring(params);
	std::string			chanName, nick;

	sstring >> chanName >> nick;
	if (sstring.fail())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "KICK"));
		return ;
	}
	if (!client.getOp(chanName))
	{
		std::cout << client.getOp(chanName) << "\n";
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		return ;
	}
	if (!client.channelNameInUse(chanName))
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		return ;
	}
	std::map<int, IRCClient *>::const_iterator	it;
	for (it = client.getClientListChannel(chanName).begin(); it != client.getClientListChannel(chanName).end(); it++)
	{
		if (it->second->getNick() == nick)
			break;
	}
	if (it == client.getClientListChannel(chanName).end())
		client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), chanName));
	else
	{
		std::cout << "--------------------------------\n";
		it->second->sendMessage(":" + client.getNick() + " KICK " + chanName + " " + it->second->getNick() + "\r\n");
		client.kickFromChannel(chanName, it->second->getFd());
	}

	
	 
	
}
