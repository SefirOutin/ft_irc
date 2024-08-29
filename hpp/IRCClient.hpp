#ifndef IRCCLIENT_HPP
#define IRCCLIENT_HPP

#include "IRCServer.hpp"

class IRCServer;

class IRCClient
{
public:
	// IRCClient() {};
	IRCClient(int fd, IRCServer *server);
	IRCClient(const IRCClient &other);
	~IRCClient();

	IRCClient &operator=(const IRCClient &other);

	const std::string &getNick() const;
	void setNick(std::string nick);
	const std::vector<std::string> getUser() const;
	void setUser(const std::string &user);
	bool getWelcom();
	void setWelcom(bool status);
	int getFd() const;
	bool isConnected() const;
	void setConnected(bool status);
	bool checkPass(const std::string &password) const;
	IRCClient getClient(const std::string &nick);
	std::string getClientInfos();
	const std::map<int, IRCClient *> getListClientChannel(const std::string &name) const;

	void sendMessage(const std::string &msg) const;
	void receiveMessages();
	bool nickAlreadyInUse(std::string arg, int clientFd);
	int leaveChannel(const std::string &name);
	void joinChannel(const std::string &name);
	void createChannel(const std::string &name);
	bool channelNameAlreadyInUse(const std::string &name);
	void sendToChannel(const std::string &message, int senderFd, const std::string &chanName);

private:
	IRCServer *_server;
	int _fd;
	bool _connected;
	bool _sendWelcom;
	std::string _nick;
	std::vector<std::string> _user;
};

#endif