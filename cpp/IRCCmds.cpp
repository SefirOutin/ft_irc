#include "IRCCmds.hpp"

bool verify_string_format(const std::string &input_string)
{
	char c;

	for (size_t i = 0; i < input_string.size(); i++)
	{
		c = input_string[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || std::string("_-[]\\`^{}").find(c) != std::string::npos))
			return (false);
		else if (i >= 9)
			return (false);
	}
	return (true);
}

bool checkArgUser(const std::string &params)
{
	int i;

	std::istringstream arg(params);
	std::string word;
	i = 0;
	while (arg >> word)
		i++;
	if (i == 4)
		return (true);
	return (false);
}

void NickCommand::execute(const std::string &params, IRCClient &client)
{
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

void PassCommand::execute(const std::string &params, IRCClient &client)
{
	client.setFirst(false);
	if (client.isConnected())
		client.sendMessage(ERR_ALREADYREGISTRED);
	else if (params.empty())
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "PASS"));
	else if (!client.checkPass(params))
		client.setConnected(false);
	else
		client.setConnected(true);
}

void UserCommand::execute(const std::string &params, IRCClient &client)
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

void CapCommand::execute(const std::string &params, IRCClient &client)
{
	(void)params;
	(void)client;
}

void PrivmsgCommand::execute(const std::string &params, IRCClient &client)
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
		(i == 0) ? nickOrChanToSend = split.erase(split.length() - 1,
																							split.length())
						 : msgToSend = split;
	}
	if (msgToSend.empty())
	{
		client.sendMessage(ERR_NOTEXTTOSEND(client.getNick()));
		return;
	}
	else if (nickOrChanToSend[0] == '#')
	{
		if (!client.inChannel(nickOrChanToSend))
		{
			client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), nickOrChanToSend));
			return;
		}
		std::string name(nickOrChanToSend);
		if (client.channelNameInUse(nickOrChanToSend))
			client.sendToChannel(client.getClientInfos() + " PRIVMSG " + name + " :" + msgToSend + "\r\n", client.getFd(),
													 nickOrChanToSend);
	}
	else if (client.nickAlreadyInUse(nickOrChanToSend, client.getFd()))
		client.getClient(nickOrChanToSend)
				.sendMessage(client.getClientInfos() + " PRIVMSG " + nickOrChanToSend + " :" + msgToSend + "\r\n");
	else
		client.sendMessage(ERR_NOSUCHNICK(client.getNick(), nickOrChanToSend));
}

void JoinCommand::execute(const std::string &params, IRCClient &client)
{
	std::istringstream sstring(params);
	std::string chanName, key;
	sstring >> chanName >> key;
	if (!params.size())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "JOIN"));
		return;
	}
	if (params[0] != '#' && params[0] != '&')
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), params));
		return;
	}
	if (client.channelNameInUse(chanName))
	{
		if (client.channelIsFull(chanName))
		{
			client.sendMessage(ERR_CHANNELISFULL(client.getNick(), chanName));
			return;
		}
		if (client.inMode(chanName, "i") && !client.isWhiteListed(client.getNick(), chanName))
		{
			client.sendMessage(ERR_INVITEONLYCHANNEL(client.getNick(),
																							 chanName));
			return;
		}
		if (client.inMode(chanName, "k"))
		{
			if (key.empty() || client.checkChannelPassword(chanName,
																										 key) == false)
			{
				client.sendMessage(ERR_BADCHANNELKEY(client.getNick(),
																						 chanName));
				return;
			}
		}
		client.joinChannel(chanName);
		client.sendToChannel(":" + client.getNick() + "!~" + client.getUser()[1] + "@" + "localhost" + " JOIN " + params, client.getFd(), params);
	}
	else
	{
		client.createChannel(chanName);
	}
	// a verifier mais c'est l'idee taff un peu connard de connaissance
	client.sendMessage(client.getClientInfos() + " JOIN " + chanName + "\r\n");
	client.sendNameReply(chanName);
}

void PartCommand::execute(const std::string &params, IRCClient &client)
{
	if (!params.size())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "PART"));
		return;
	}
	switch (client.leaveChannel(params))
	{
	case 0:
		client.sendMessage(client.getClientInfos() + " PART " + params + "\r\n");
		// message optionnel a rajouter (raison du leave) -> petit parsing a faire, comme pour kick
		client.sendToChannel(":" + client.getNick() + "!~" + client.getUser()[1] + "@" + "localhost" + " PART " + params, client.getFd(), params);
		break;
	case 1:
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), params));
		break;
	case 2:
		client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), params));
		break;
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
		return;
	}
	std::getline(sstring >> std::ws, msg);
	switch (client.kickFromChannel(chanName, nick, msg))
	{
	case 1:
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		break;
	case 2:
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		break;
	case 3:
		client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), chanName));
		break;
	case 4:
		client.sendMessage(ERR_USERNOTINCHANNEL(client.getNick(), nick,
																						chanName));
		break;
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
		return;
	}
	if (!client.nickAlreadyInUse(nick, client.getFd()))
	{
		client.sendMessage(ERR_NOSUCHNICK(client.getNick(), nick));
		return;
	}
	if (!client.channelNameInUse(chanName))
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		return;
	}
	if (client.inMode(chanName, "i") && !client.getOp(chanName))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		return;
	}
	client.whiteList(nick, chanName);
	client.getClient(nick).sendMessage(":" + client.getNick() + " INVITE " + nick + " " + chanName + "\r\n");
}

void TopicCommand::execute(const std::string &params, IRCClient &client)
{
	IRCChannel *channel;

	std::istringstream sstring(params);
	std::string chanName, topic;
	sstring >> chanName;
	if (sstring.fail())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "TOPIC"));
		return;
	}
	channel = client.findChannel(chanName);
	if (!channel)
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		return;
	}
	if (!client.inChannel(chanName))
	{
		client.sendMessage(ERR_NOTONCHANNEL(client.getNick(), chanName));
		return;
	}
	std::getline(sstring >> std::ws, topic);
	if (topic.empty())
	{
		client.sendMessage(":" + client.getNick() + " TOPIC " + chanName + " " + channel->getTopic() + "\r\n");
		return;
	}
	if (client.inMode(chanName, "t") && !client.getOp(chanName))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		return;
	}
	client.setTopic(chanName, topic);
	client.sendToChannelMode(":" + client.getNick() + "!" + client.getUser()[0] + " TOPIC " + chanName + " " + topic + "\r\n", chanName);
}

std::string getMode(const std::string &chanName, IRCClient &client)
{
	std::string result;
	if (client.inMode(chanName, "i"))
		result += "i";
	if (client.inMode(chanName, "t"))
		result += "t";
	if (client.inMode(chanName, "o"))
		result += "o";
	if (client.inMode(chanName, "k"))
		result += "k";
	if (client.inMode(chanName, "l"))
		result += "l";
	if (result.empty())
		result = "no mode";
	return (result);
}

bool isNumber(const std::string &str)
{
	if (str.empty())
		return (false);
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!std::isdigit(*it))
			return (false);
	}
	return (true);
}

bool isGreaterThanIntMax(const std::string &str)
{
	char *endPtr;
	long number;

	if (!isNumber(str))
		return (false);
	errno = 0;
	number = std::strtol(str.c_str(), &endPtr, 10);
	if (errno == ERANGE || number > 2147483647)
		return (true);
	return (false);
}

int modeIT(IRCClient &client, const std::string &chanName,
					 const std::string &mode)
{
	if (mode.find_first_of("it") != std::string::npos)
		client.setMode(chanName, mode);
	return (0);
}

int modeK(IRCClient &client, const std::string &chanName,
					const std::string &mode, const std::string &params)
{
	if (mode[0] == '+')
	{
		client.setKey(chanName, params);
		client.setMode(chanName, mode);
	}
	else
		client.setKey(chanName, "");
	return (0);
}

int modeO(IRCClient &client, const std::string &chanName,
					const std::string &mode, const std::string &params)
{
	if (mode[0] == '+')
		client.changeOpe(chanName, params, true);
	else
		client.changeOpe(chanName, params, false);
	return (0);
}

int modeL(IRCClient &client, const std::string &chanName,
					const std::string &mode, const std::string &params)
{
	if (mode[0] == '-')
		client.findChannel(chanName)->setUserLimit(-1);
	else
	{
		if (isGreaterThanIntMax(params))
			client.findChannel(chanName)->setUserLimit(2147483647);
		else
			client.findChannel(chanName)->setUserLimit(std::atoi(params.c_str()));
		client.setMode(chanName, mode);
	}
	return (0);
}

void ModeCommand::execute(const std::string &params, IRCClient &client)
{
	char currentMode;
	char currentSign;

	std::istringstream sstring(params);
	std::string chanName, mode;
	sstring >> chanName >> mode;
	if (!client.getFirst())
	{
		client.setFirst(true);
		return;
	}
	if (chanName.empty())
	{
		client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(), "MODE"));
		return;
	}
	if (!client.channelNameInUse(chanName))
	{
		client.sendMessage(ERR_NOSUCHCHANNEL(client.getNick(), chanName));
		return;
	}
	if (mode.empty())
	{
		std::string currentModes = getMode(chanName, client);
		client.sendMessage(RPL_CHANNELMODEIS(client.getNick(), chanName,
																				 currentModes));
		return;
	}
	if (!client.getOp(chanName))
	{
		client.sendMessage(ERR_CHANOPRIVSNEEDED(client.getNick(), chanName));
		return;
	}
	if (mode.empty() || (mode[0] != '+' && mode[0] != '-'))
	{
		client.sendMessage(ERR_UNKNOWNMODE(client.getNick(), mode));
		return;
	}
	std::string modeParam;
	currentSign = '+';
	for (size_t i = 0; i < mode.size(); ++i)
	{
		currentMode = mode[i];
		if (currentMode == '+' || currentMode == '-')
		{
			currentSign = currentMode;
			continue;
		}
		if (currentMode == 'i' || currentMode == 't')
		{
			if (modeIT(client, chanName, std::string(1, currentSign) + currentMode) == -1)
				return;
		}
		else if (currentMode == 'o' || currentMode == 'k' || currentMode == 'l')
		{
			if (!(sstring >> modeParam))
			{
				client.sendMessage(ERR_NEEDMOREPARAMS(client.getNick(),
																							"MODE"));
				return;
			}
			if (currentMode == 'o')
			{
				if (modeO(client, chanName, std::string(1, currentSign) + currentMode, modeParam) == -1)
					return;
			}
			else if (currentMode == 'k')
			{
				if (modeK(client, chanName, std::string(1, currentSign) + currentMode, modeParam) == -1)
					return;
			}
			else if (currentMode == 'l')
			{
				if (modeL(client, chanName, std::string(1, currentSign) + currentMode, modeParam) == -1)
					return;
			}
		}
		else
		{
			client.sendMessage(ERR_UNKNOWNMODE(client.getNick(),
																				 std::string(1, currentMode)));
			return;
		}
	}
	client.sendToChannelMode(":" + client.getNick() + "!" + client.getUser()[0] + " MODE " + chanName + " " + mode + "\r\n", chanName);
}

void QuitCommand::execute(const std::string &params, IRCClient &client)
{
	(void)params;
	client.closeClientConnection();
}
