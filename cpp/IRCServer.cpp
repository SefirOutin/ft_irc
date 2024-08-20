/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 19:34:36 by soutin            #+#    #+#             */
/*   Updated: 2024/08/14 16:27:07 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"

IRCServer::IRCServer(int port, const std::string &password)
		: _password(password)
{
	createServerSocket(port);
}

IRCServer::~IRCServer()
{
	for (size_t i = 0; i < _fds.size(); i++)
		close(_fds[i].fd);
}

void	IRCServer::socketOpt()
{
	int on = 1;
	
	if (setsockopt(_sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on)) < 0)
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

void IRCServer::createServerSocket(int port)
{
	struct pollfd		pollFd;
	int 				on = 1;
	struct sockaddr_in sockAddr;
	
	_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockFd < 0)
	{
		std::cerr << "socket error\n";
		exit(-1);
	}
	pollFd.fd = _sockFd;
	pollFd.events = POLLIN;
	_fds.push_back(pollFd);
	if (setsockopt(_sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on)) < 0)
	{
		std::cerr << "setsockopt() failed\n";
		exit(-1);
	}
	if (ioctl(_sockFd, FIONBIO, (char *)&on) < 0)
	{
		std::cerr << "ioctl() failed\n";
		exit(-1);
	}
	bzero(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_sockFd, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0)
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
}

int IRCServer::start()
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
					acceptConnections();
				else
					receivedData(_fds[i].fd);
			}
		}
	}
	return (0);
}

int IRCServer::acceptConnections()
{
	struct sockaddr_in clientSockAddr;
	struct pollfd clientPollFd;
	socklen_t addrLen;
	int new_connection;

	addrLen = sizeof(clientSockAddr);
	new_connection = accept(_sockFd, (struct sockaddr *)&clientSockAddr, &addrLen);
	if (new_connection < 0)
	{
		std::cerr << "accept failed" << "\n";
		return (1);
	}
	// std::cout << "accept fd: " << new_connection << "\n";
	clientPollFd.fd = new_connection;
	clientPollFd.events = POLLIN;
	_fds.push_back(clientPollFd);

	IRCClientHandler *cli = new IRCClientHandler(new_connection, _commandParser);
	_clients.insert(std::pair<int, IRCClientHandler*>(new_connection, cli));
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
}

void	IRCServer::closeConnection(int clientFd)
{
	std::map<int, IRCClientHandler*>::iterator mapIt = _clients.find(clientFd);
	if (mapIt != _clients.end())
		_clients.erase(clientFd);
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == clientFd)
			_fds.erase(_fds.begin() + i);
	}
	close(clientFd);
}

void IRCServer::receivedData(int clientFd)
{
	char buffer[1024];
	int nbytes;

	nbytes = recv(clientFd, buffer, 1024, 0);
	if (nbytes <= 0)
	{
		if (nbytes == 0)
			std::cout << "Connection closed" << std::endl;
		else
			std::cerr << "recv failed" << "\n";
		closeConnection(clientFd);
		return;
	}
	buffer[nbytes] = '\0';
	if (nbytes > 510)
		std::cerr << "too many char\n";
	// std::cout <<buffer << "\n";
	if (!getCmd(buffer, clientFd))
		return;
}
int IRCServer::getCmd(std::string buff, int clientFd)
{
	std::string line;
	std::stringstream buffer(buff);

	int nLine = std::count(buff.begin(), buff.end(), '\n');
	while (nLine--)
	{
		if (std::getline(buffer, line))
		{
			std::string arg;
			size_t posLastCr = line.find("\r");
			size_t posFirstSpace = line.find(" ");
			std::string cmd = line.substr(0, posFirstSpace);
			if (posFirstSpace != arg.npos)
			{
				arg = line.substr(posFirstSpace + 1, posLastCr);
				arg.erase(std::remove(arg.begin(), arg.end(), '\r'), arg.end());
			}
			// std::cout << cmd << " " << arg << std::endl;
			handleData(cmd, arg, clientFd);
		}
	}
	return (0);
}
int IRCServer::handleData(std::string cmd, std::string arg, int clientFd)
{
	(void)arg;
	(void)clientFd;
	if (!cmd.compare("PASS"))
		return 0;
		// PASS(&_clients, arg, _password, clientFd);
	// else if (!cmd.compare("NICK"))
	// 	NICK(&_clients, arg, clientFd);
	// else if (!cmd.compare("USER"))
	// 	USER(&_clients, arg, clientFd);
	// else if (!cmd.compare("JOIN"))
	// JOIN(arg, index);
	return (0);
}


