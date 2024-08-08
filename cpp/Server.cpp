/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 19:34:36 by soutin            #+#    #+#             */
/*   Updated: 2024/08/08 19:02:00 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, const std::string &password)
		: _password(password + '\r')
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

int Server::startServer()
{
	struct pollfd pollFd;
	int on;

	on = 1;
	_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockFd < 0)
	{
		std::cerr << "socket error\n";
		return (1);
	}
	if (setsockopt(_sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,
								 sizeof(on)) < 0)
	{
		std::cerr << "setsockopt() failed\n";
		close(_sockFd);
		exit(-1);
	}
	if (ioctl(_sockFd, FIONBIO, (char *)&on) < 0)
	{
		std::cerr << "ioctl() failed\n";
		close(_sockFd);
		exit(-1);
	}
	if (bind(_sockFd, (struct sockaddr *)&_sockAddr, sizeof(_sockAddr)) < 0)
	{
		std::cerr << "bind error\n";
		return (1);
	}
	if (listen(_sockFd, SOMAXCONN) < 0)
	{
		std::cerr << "listen error\n";
		return (1);
	}
	std::cout << "Listens ...\n";
	pollFd.fd = _sockFd;
	pollFd.events = POLLIN;
	_fds.push_back(pollFd);
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
					receivedData(i);
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
	new_connection = accept(_sockFd, (struct sockaddr *)&clientSockAddr,
													&addrLen);
	if (new_connection < 0)
	{
		std::cerr << "accept failed"
							<< "\n";
		return (1);
	}
	clientPollFd.fd = new_connection;
	clientPollFd.events = POLLIN;
	_fds.push_back(clientPollFd);
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
}

int Server::getCmd(std::string buff, size_t index)
{
	std::string line;
	std::stringstream buffer(buff);

	int nLine = std::count(buff.begin(), buff.end(), '\n');
	while (nLine--)
	{
		if (std::getline(buffer, line))
		{
			size_t posFirstSpace = line.find(" ");
			std::string cmd = line.substr(0, posFirstSpace);
			std::string arg = line.substr(posFirstSpace + 1);
			handleData(cmd, arg, index);
		}
	}
	return 0;
}

void Server::receivedData(size_t index)
{
	char buffer[1024];
	int nbytes;

	nbytes = recv(_fds[index].fd, buffer, 1024, 0);
	if (nbytes <= 0)
	{
		if (nbytes == 0)
		{
			std::cout << "Connection closed" << index << std::endl;
			_clients.erase(_clients.begin() + index - 1);
		}
		else
			std::cerr << "recv failed"
								<< "\n";
		close(_fds[index].fd);
		_fds.erase(_fds.begin() + index);
		return;
	}
	buffer[nbytes] = '\0';
	if (nbytes > 510)
		std::cerr << "too many char\n";
	// std::cout <<buffer << "\n";
	if (!getCmd(buffer, index))
		return;
}
int Server::handleData(std::string cmd, std::string arg, size_t index)
{
	if (_clients.size() < index)
	{
		Client cli;
		_clients.push_back(cli);
	}
	if (!cmd.compare("PASS"))
		PASS(arg, index);
	else if (!cmd.compare("NICK"))
		NICK(arg, index);
	else if (!cmd.compare("USER"))
		USER(arg, index);
	return (0);
}

void Server::PASS(std::string arg, size_t index)
{
	_clients[index - 1].setPass(arg);
}
void Server::NICK(std::string arg, size_t index)
{
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].getNick() == arg)
		{
			std::cout << "----" << std::endl;
			std::string buff = ": 433 bilel : Nickname is already in use\r\n";
			send(_fds[index].fd, buff.c_str(), buff.size(), 0);
			_clients.erase(_clients.begin() + i);
			return;
		}
	}
	if (_clients[index - 1].getPass().empty())
	{
		std::string buff = ": 451 " + arg + " : You have not registered!\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
	}
	else
		_clients[index - 1].setNick(arg);
}
void Server::USER(std::string arg, size_t index)
{
	_clients[index - 1].setUser(arg);
	if (!_clients[index - 1].getPass().compare(_password) && !_clients[index - 1].getNick().empty())
	{
		_clients[index - 1].setUser(arg);
		std::string buff = ": 001 " + _clients[index - 1].getNick() + " : Welcome to the IRC server!\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
	}
}
