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
	else if (params.empty())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "PRIVMSG"));
		return;
	}
	std::string split, nickOrChanToSend, msgToSend;
	std::istringstream arg(params);
	for (size_t i = 0; std::getline(arg, split, ':'); i++)
	{
		if (split.empty() && i == 0)
		{
			std::cout << "nick or chan : " << split << std::endl;
			client.sendMessage(ERR_NORECIPIENT(client.getNick(), "PRIVMSG"));
			return;
		}
		(i == 0) ? nickOrChanToSend = split.erase(split.length() - 1, split.length()) : msgToSend = split;
	}
	if (msgToSend.empty())
	{
		client.sendMessage(ERR_NOTEXTTOSEND(client.getNick()));
		return;
	}
	else if (nickOrChanToSend[0] == '#')
	{
		std::string name(nickOrChanToSend);
		// nickOrChanToSend.erase(0, 1);

		if (client.channelNameInUse(nickOrChanToSend))
			client.sendToChannel(client.getClientInfos() + " PRIVMSG " + name + " :" + msgToSend + "\r\n", client.getFd(), nickOrChanToSend);
	}
	else if (client.nickAlreadyInUse(nickOrChanToSend, client.getFd()))
		client.getClient(nickOrChanToSend)
				.sendMessage(client.getClientInfos() + " PRIVMSG " + nickOrChanToSend + " :" + msgToSend + "\r\n");
	else
		client.sendMessage(ERR_NOSUCHNICK(client.getNick(), nickOrChanToSend));
	// PRIVMSG bilel : ca va
	// : bi!~bmoudach@localhost PRIVMSG bilel : ca va
}

void JoinCommand::execute(const std::string &params, IRCClient &client)
{
	std::istringstream sstring(params);
	std::string chanName, key;
	sstring >> chanName >> key;
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
	if (client.channelNameInUse(chanName))
	{
		if (client.channelIsFull(chanName))
		{
			client.sendMessage(ERR_CHANNELISFULL(client.getNick(), chanName));
			return ;
		}
		if (client.inMode(chanName, "i")
			&& !client.isWhiteListed(client.getNick(), chanName))
		{
			client.sendMessage(ERR_INVITEONLYCHANNEL(client.getNick(),
						chanName));
			return ;
		}
		if (client.inMode(chanName, "k"))
		{
			if (key.empty() || client.checkChannelPassword(chanName,
					key) == false)
			{
				client.sendMessage(ERR_BADCHANNELKEY(client.getNick(),
							chanName));
				return ;
			}
		}
		client.joinChannel(chanName);
	}
	else
	{
		client.createChannel(chanName);
	}
	client.sendMessage(client.getClientInfos() + " JOIN " + chanName + "\r\n");
	client.sendNameReply(chanName);
}

void PartCommand::execute(const std::string &params, IRCClient &client)
{
	if (!params.size())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "PART"));
		return ;
	}
	switch (client.leaveChannel(params))
	{
	case 0:
		client.sendMessage(client.getClientInfos() + " PART " + params
				+ "\r\n");
		break ;
	case 1:
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), params));
		break ;
	case 2:
		client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), params));
		break ;
	}
}

void KickCommand::execute(const std::string &params, IRCClient &client)
{
	std::istringstream sstring(params);
	std::string chanName, nick, msg;
	sstring >> chanName >> nick;
	if (sstring.fail())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "KICK"));
		return ;
	}
	std::getline(sstring >> std::ws, msg);
	switch (client.kickFromChannel(chanName, nick, msg))
	{
	case 1:
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		break ;
	case 2:
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		break ;
	case 3:
		client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), chanName));
		break ;
	case 4:
		client.sendMessage(ERR_USERNOTINCHANNEL(client.getNick(), nick,
					chanName));
		break ;
	}
}

void InviteCommand::execute(const std::string &params, IRCClient &client)
{
	std::istringstream sstring(params);
	std::string nick, chanName;
	sstring >> nick >> chanName;
	if (sstring.fail())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "INVITE"));
		return ;
	}
	if (!client.nickAlreadyInUse(nick, client.getFd()))
	{
		client.sendMessage(ERR_NOSUCHNICK(client.getNick(), nick));
		return ;
	}
	if (!client.channelNameInUse(chanName))
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		return ;
	}
	if (client.inMode(chanName, "i") && !client.getOp(chanName))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		return ;
	}
	client.whiteList(nick, chanName);
	client.getClient(nick).sendMessage(":" + client.getNick() + " INVITE "
			+ nick + " " + chanName + "\r\n");
}

void TopicCommand::execute(const std::string &params, IRCClient &client)
{
	IRCChannel	*channel;

	std::istringstream sstring(params);
	std::string chanName, topic;
	sstring >> chanName;
	if (sstring.fail())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "TOPIC"));
		return ;
	}
	channel = client.findChannel(chanName);
	if (!channel)
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		return ;
	}
	std::getline(sstring >> std::ws, topic);
	if (topic.empty())
	{
		client.sendMessage(":" + client.getNick() + " TOPIC " + chanName + " "
				+ channel->getTopic() + "\r\n");
		return ;
	}
	if (client.inMode(chanName, "t") && !client.getOp(chanName))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		return ;
	}
	client.setTopic(chanName, topic);
	client.sendMessage(":" + client.getNick() + " TOPIC " + chanName + " "
			+ topic + "\r\n");
}

std::string getMode(const std::string &chanName, IRCClient &client)
{
	std::string result;
	for (size_t i = 0; i < 5; ++i)
	{
		if (client.inMode(chanName, "i"))
			result += "i";
		else if (client.inMode(chanName, "t"))
			result += "t";
		else if (client.inMode(chanName, "o"))
			result += "o";
		else if (client.inMode(chanName, "k"))
			result += "k";
		else if (client.inMode(chanName, "l"))
			result += "l";
	}
	if (result.empty())
		result = "no mode";
	return result;
}

void ModeCommand::execute(const std::string &params, IRCClient &client)
{
	std::istringstream sstring(params);
    std::string chanName, mode;
    sstring >> chanName;

	if (client.getFirst() == false)
	{
		client.setFirst(true);
		return;
	}

    if (chanName.empty())
    {
        client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "MODE"));
        return;
    }

    sstring >> mode;
    if (mode.empty())
    {
        std::string currentModes = getMode(chanName, client);
        client.sendMessage(RPL_CHANNELMODEIS(client.getNick(), chanName, currentModes));
        return;
    }

	if (!client.channelNameInUse(chanName))
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		return ;
	}

    if (client.leaveChannel(chanName) == 2)
    {
        client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), chanName));
        return;
    }

	if (!client.getOp(chanName))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		return ;
	}
	if (mode.empty() || (mode[0] != '+' && mode[0] != '-'))
	{
		client.sendMessage(ERR_UNKNOWNMODE(client.getNick(), mode));
		return ;
	}
	for (size_t i = 0; i < mode.size(); ++i)
	{
		if (std::string("-+itkol").find(mode[i]) == std::string::npos)
		{
			client.sendMessage(ERR_UNKNOWNMODE(client.getNick(), mode));
			return ;
		}
	}
	if (mode.find_first_of("it") != std::string::npos)
		client.setMode(chanName, mode);
	else if (mode.find_first_of("kol") != std::string::npos)
	{
		client.setMode(chanName, mode);
		std::string modeParam;
		sstring >> modeParam;
		if (mode.find_first_of("k") != std::string::npos)
		{
			if (sstring.fail())
			{
				client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(),
							"MODE"));
				return ;
			}
			if (mode[0] == '+')
				client.setKey(chanName, modeParam);
			else
				client.setKey(chanName, "");
		}
		else if (mode.find_first_of("o") != std::string::npos)
		{
			if (sstring.fail())
			{
				client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(),
							"MODE"));
				return ;
			}
			if (mode[0] == '+')
				client.changeOpe(chanName, modeParam, true);
			else
				client.changeOpe(chanName, modeParam, false);
		}
		else //mode l
		{
			if (mode[0] == '-')
				client.findChannel(chanName)->setUserLimit(-1);
			else
				client.findChannel(chanName)->setUserLimit(std::atoi(modeParam.c_str()));
		}
	}
	client.sendMessage(":" + client.getNick() + " MODE " + chanName + " " + mode
			+ "\r\n");
}
