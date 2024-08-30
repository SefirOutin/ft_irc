#include "IRCServer.hpp"

IRCServer::IRCServer(int port, const std::string &password):
	_password(password)
{
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
}

IRCServer::~IRCServer()
{
	for (size_t i = 0; i < _fds.size(); i++)
		close(_fds[i].fd);
}

const std::string &IRCServer::getPass() const
{
 	 return (_password);
}

const std::map<int, IRCClient> &IRCServer::getClients() const
{
 	 return (_clients);
}

const std::map<std::string, IRCChannel>	&IRCServer::getChannels() const
{
  	return (_channels);
}

void IRCServer::removeChannel(std::string name)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(name);
	if (it != _channels.end())
		_channels.erase(name);
}

void IRCServer::socketOpt()
{
	int on = 1;

	if (setsockopt(_sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
	{
		std::cerr << "setsockopt() failed\n";
		exit(-1);
	}
	if (ioctl(_sockFd, FIONBIO, (char *)&on) < 0)
	{
		std::cerr << "ioctl() failed\n";
		exit(-1);
	}
}

int IRCServer::startServer()
{
	struct pollfd pollFd;

	_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockFd < 0)
	{
		std::cerr << "socket error\n";
		exit(-1);
	}
	pollFd.fd = _sockFd;
	pollFd.events = POLLIN;
	_fds.push_back(pollFd);
	socketOpt();
	if (bind(_sockFd, (struct sockaddr *)&_sockAddr, sizeof(_sockAddr)) < 0)
	{
		std::cerr << "bind error\n";
		exit(-1);
	}
	if (listen(_sockFd, SOMAXCONN) < 0)
	{
		std::cerr << "listen error\n";
		exit(-1);
	}
	std::cout << "Listens ...\n";
	return (0);
}

int IRCServer::run()
{
	int poll_count;

	while (1)
	{
		poll_count = poll(_fds.data(), _fds.size(), -1);
		if (poll_count < 0)
		{
			std::cerr << "poll error\n";
			return (1);
		}
		for (size_t i = 0; i < _fds.size(); ++i)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _sockFd)
				{
					acceptConnections();
				}
				else
				{
					_clients.find(_fds[i].fd)->second.receiveMessages();
				}
			}
		}
		// if (_channels.find("ok") != _channels.end())
		// std::cout << "nbUser in #ok: " << _channels.find("ok")->second.getNbUser() << "\n";
		// std::cout << "nbChannels: " << _channels.size() << "\n";
	}
  return (0);
}

int IRCServer::acceptConnections()
{
	struct sockaddr_in clientSockAddr;
	struct pollfd clientPollFd;
	socklen_t addrLen;
	int new_connection;

	bzero(&clientPollFd, sizeof(pollfd));
	addrLen = sizeof(clientSockAddr);
	new_connection = accept(_sockFd, (struct sockaddr *)&clientSockAddr, &addrLen);
	if (new_connection < 0)
	{
		std::cerr << "accept failed" << "\n";
		return (1);
	}
	clientPollFd.fd = new_connection;
	clientPollFd.events = POLLIN;
	_fds.push_back(clientPollFd);
	_clients.insert(std::pair<int, IRCClient>(new_connection, IRCClient(new_connection, this)));
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
}

void IRCServer::parseCmds(const std::string &buff, IRCClient &client)
{

	std::stringstream buffer(buff);
	std::string line;
	while (std::getline(buffer, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
		{
		line.erase(line.size() - 1);
		}
		size_t posFirstSpace = line.find(' ');
		std::string cmd = line.substr(0, posFirstSpace);
		std::string arg = (posFirstSpace != std::string::npos) ? line.substr(posFirstSpace + 1) : "";
		std::map<std::string, IRCCommandHandler *>::iterator it = _cmds.find(cmd);
		if (it != _cmds.end())
		{
		it->second->execute(arg, client);
		}
		else
			client.sendMessage(ERR_UNKNOWNCOMMAND(cmd));
	}
}

void IRCServer::closeConnection(int clientFd)
{
  std::map<int, IRCClient>::iterator mapIt = _clients.find(clientFd);
  if (mapIt != _clients.end())
    _clients.erase(clientFd);
  for (size_t i = 0; i < _fds.size(); i++)
  {
    if (_fds[i].fd == clientFd)
      _fds.erase(_fds.begin() + i);
  }
  close(clientFd);
  std::cout << "Connection closed\n";
}

void	IRCServer::newChannel(const std::string &name, IRCClient &op)
{
	// IRCChannel	channel(name, op);
	_channels.insert(std::pair<std::string, IRCChannel>(name, IRCChannel(name, op)));
	
}

void	IRCServer::newConnectionToChannel(const std::string &name, IRCClient &client)
{
	std::map<std::string, IRCChannel>::iterator it = _channels.find(name);
	it->second.newConnection(client);
}

void	IRCServer::removeClientFromChannel(const std::string &name, const int clientFd)
{
	std::map<std::string, IRCChannel>::iterator	it = _channels.find(name);
	it->second.removeUser(clientFd);
	if (!it->second.getNbUser())
		removeChannel(name);
	std::map<int, IRCClient>::iterator	itClient = _clients.find(clientFd);
	itClient->second.setOp(name, true, true);
}