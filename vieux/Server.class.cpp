/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 19:34:36 by soutin            #+#    #+#             */
/*   Updated: 2024/08/13 18:12:17 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.class.hpp"

Server::Server(int port, const std::string &password)
		: _password(password)
{
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_port = htons(port);
	_sockAddr.sin_addr.s_addr = INADDR_ANY;
}

Server::~Server()
{
	for (size_t i = 0; i < _fds.size(); i++)
		close(_fds[i].fd);
}

void	Server::socketOpt()
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

int Server::startServer()
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

int Server::run()
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

int Server::acceptConnections()
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
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
}

void	Server::closeConnection(int clientFd)
{
	std::map<int, Client>::iterator mapIt = _clients.find(clientFd);
	if (mapIt != _clients.end())
		_clients.erase(clientFd);
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == clientFd)
			_fds.erase(_fds.begin() + i);
	}
	close(clientFd);
}

void Server::receivedData(int clientFd)
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
int Server::getCmd(std::string buff, int clientFd)
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
int Server::handleData(std::string cmd, std::string arg, int clientFd)
{
	if (_clients.find(clientFd) == _clients.end())
	{
		Client cli;
		_clients.insert(std::pair<int, Client>(clientFd, cli));
	}
	if (!cmd.compare("PASS"))
		PASS(&_clients, arg, _password, clientFd);
	else if (!cmd.compare("NICK"))
		NICK(&_clients, arg, clientFd);
	else if (!cmd.compare("USER"))
		USER(&_clients, arg, clientFd);
	// else if (!cmd.compare("JOIN"))
	// JOIN(arg, index);
	return (0);
}


