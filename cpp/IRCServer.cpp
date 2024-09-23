#include "IRCServer.hpp"

volatile sig_atomic_t IRCServer::_signal = false;

IRCServer::IRCServer(int port, const std::string &password)
	: _password(password)
{
	_signal = false;
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_port = htons(port);
	_sockAddr.sin_addr.s_addr = INADDR_ANY;
	_cmds["PASS"] = new PassCommand();
	_cmds["NICK"] = new NickCommand();
	_cmds["USER"] = new UserCommand();
	_cmds["PING"] = new PingCommand();
	_cmds["CAP"] = new CapCommand();
	_cmds["PRIVMSG"] = new PrivmsgCommand();
	_cmds["JOIN"] = new JoinCommand();
	_cmds["PART"] = new PartCommand();
	_cmds["KICK"] = new KickCommand();
	_cmds["INVITE"] = new InviteCommand();
	_cmds["TOPIC"] = new TopicCommand();
	_cmds["MODE"] = new ModeCommand();
	_cmds["QUIT"] = new QuitCommand();
}

IRCServer::~IRCServer()
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, _fds[i].data.fd, NULL);
		close(_fds[i].data.fd);
	}
	
	std::map<std::string, IRCCommandHandler *>::iterator	cmdIt;
	for (cmdIt = _cmds.begin(); cmdIt != _cmds.end(); cmdIt++)
		delete cmdIt->second;
    close(_sockFd);
    close(_epollFd);
}

const std::string &IRCServer::getPass() const
{
	return (_password);
}

const std::map<int, IRCClient> &IRCServer::getClients() const
{
	return (_clients);
}

const std::map<std::string, IRCChannel> &IRCServer::getChannels() const
{
	return (_channels);
}

void IRCServer::removeChannel(std::string name)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(name);
	if (it != _channels.end())
		_channels.erase(name);
}

int IRCServer::socketOpt()
{
	int	on = 1;

	if (setsockopt(_sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,
			sizeof(on)) < 0)
		return (1);
	if (ioctl(_sockFd, FIONBIO, (char *)&on) < 0)
		return (1);
	return (0);
}

int IRCServer::startServer()
{
	struct epoll_event	pollFd;

	bzero(&pollFd, sizeof(pollFd));
	_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockFd < 0)
		return (1);
	_epollFd = epoll_create1(0);
	if (_epollFd < 0)
		return (1);
	pollFd.data.fd = _sockFd;
	pollFd.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _sockFd, &pollFd) < 0)
		return (1);
	if (socketOpt() == 1)
		return (1);
	if (bind(_sockFd, (struct sockaddr *)&_sockAddr, sizeof(_sockAddr)) < 0)
		return (1);
	if (listen(_sockFd, SOMAXCONN) < 0)
		return (1);
	signal(SIGINT, IRCServer::signalHandler);
	signal(SIGTERM, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	std::cout << "Listens ...\n";
	return (0);
}

int IRCServer::run()
{
	int	epoll_count;
	int	fd;

	_fds.resize(64);
	while (!_signal)
	{
		epoll_count = epoll_wait(_epollFd, _fds.data(), _fds.size(), -1);
		if (epoll_count < 0 && !_signal)
		{
			perror("epoll_wait");
			return (1);
		}
		for (int i = 0; i < epoll_count; ++i)
		{
			fd = _fds[i].data.fd; 
			if ((_fds[i].events & EPOLLHUP || _fds[i].events & EPOLLERR))
				closeConnection(fd);
			if (_fds[i].events & POLLIN)
			{
				if (fd == _sockFd)
					acceptConnections();
				else
					_clients.find(fd)->second.receiveMessages();
			}
		}
	}
	return (0);
}

int IRCServer::acceptConnections()
{
	struct sockaddr_in	clientSockAddr;
	struct epoll_event	clientPollFd;
	socklen_t			addrLen;
	int					new_connection;

	bzero(&clientPollFd, sizeof(pollfd));
	addrLen = sizeof(clientSockAddr);
	new_connection = accept(_sockFd, (struct sockaddr *)&clientSockAddr,
			&addrLen);
	if (new_connection < 0)
	{
		std::cerr << "accept failed"
					<< "\n";
		return (1);
	}
	clientPollFd.data.fd = new_connection;
	clientPollFd.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, new_connection, &clientPollFd) < 0)
		return (1);
	_clients.insert(std::pair<int, IRCClient>(new_connection, IRCClient(new_connection, this)));
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
}

void IRCServer::parseCmds(const std::string &buff, IRCClient &client)
{
	size_t	posFirstSpace;

	std::stringstream buffer(buff);
	std::string line;
	while (std::getline(buffer, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
		{
			line.erase(line.size() - 1);
		}
		posFirstSpace = line.find(' ');
		std::string cmd = line.substr(0, posFirstSpace);
		std::string arg = (posFirstSpace != std::string::npos) ? line.substr(posFirstSpace
				+ 1) : "";
		std::map<std::string,
					IRCCommandHandler *>::iterator it = _cmds.find(cmd);
		if (it != _cmds.end())
			it->second->execute(arg, client);
		else
			client.sendMessage(ERR_UNKNOWNCOMMAND(cmd));
	}
}

void IRCServer::closeConnection(int clientFd)
{
	std::map<int, IRCClient>::iterator it = _clients.find(clientFd);
	if (it != _clients.end())
	{
		it->second.leaveAllChannels();
		_clients.erase(clientFd);
	}
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].data.fd == clientFd)
			_fds.erase(_fds.begin() + i);
	}
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL);
	close(clientFd);
	std::cout << "Connection closed\n";
}

void IRCServer::newChannel(const std::string &name, IRCClient &op)
{
	IRCChannel channel(name, op);
	_channels.insert(std::pair<std::string, IRCChannel>(name, channel));
}

void IRCServer::newConnectionToChannel(const std::string &name,
										IRCClient &client)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(name);
	it->second.newConnection(client);
}

void IRCServer::removeClientFromChannel(const std::string &name, const int clientFd)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(name);
	it->second.removeUser(clientFd);
	if (!it->second.getNbUser())
		removeChannel(name);
	std::map<int, IRCClient>::iterator itClient = _clients.find(clientFd);
	itClient->second.setOp(name, true, true);
}

void IRCServer::sendToChannel(const std::string &message, int senderFd,
		const std::string &chanName)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	it->second.sendToChannel(message, senderFd);
}

void IRCServer::sendToChannelMode(const std::string &message, const std::string &chanName)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	it->second.sendToChannelMode(message);
}

void IRCServer::setTopic(const std::string &chanName, const std::string &topic)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	it->second.setTopic(topic);
}

IRCChannel *IRCServer::findChannel(const std::string &chanName)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	if (it != this->getChannels().end())
		return (&it->second);
	return (NULL);
}

void IRCServer::setMode(const std::string &chanName, const std::string &mode)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	it->second.setMode(mode);
}

bool IRCServer::inMode(const std::string &chanName, const std::string &mode)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	return (it->second.inMode(mode));
}

void IRCServer::whiteList(const int &fd, const std::string &chanName)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	it->second.whiteList(fd);
}

bool IRCServer::isWhiteListed(const int &fd, const std::string &chanName)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	return (it->second.isWhiteListed(fd));
}

void IRCServer::changeOpe(const std::string &chanName, const std::string &nick,
		bool op)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	it->second.changeOpe(nick, op);
}

void IRCServer::setKey(const std::string &chanName, const std::string &key)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(chanName);
	it->second.setKey(key);
}

void	IRCServer::signalHandler(int signum)
{
	(void)signum;
	_signal = true;
}
