/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 19:34:36 by soutin            #+#    #+#             */
/*   Updated: 2024/08/05 14:16:38 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, const std::string &password) : _password(password)
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
	int on = 1;

	_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockFd < 0)
	{
		std::cerr << "socket error\n";
		return (1);
	}
	if (setsockopt(_sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
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
		return 1;
	}
	if (listen(_sockFd, SOMAXCONN) < 0)
	{
		std::cerr << "listen error\n";
		return 1;
	}
	std::cout << "Listens ...\n";
	pollFd.fd = _sockFd;
	pollFd.events = POLLIN;
	_fds.push_back(pollFd);
	return (0);
}

int Server::run()
{
	while (1)
	{
		int poll_count = poll(_fds.data(), _fds.size(), -1);
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
	new_connection = accept(_sockFd, (struct sockaddr *)&clientSockAddr, &addrLen);
	if (new_connection < 0)
	{
		std::cerr << "accept failed" << "\n";
		return (1);
	}
	clientPollFd.fd = new_connection;
	clientPollFd.events = POLLIN;
	_fds.push_back(clientPollFd);
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
}

std::string parseValue(const std::string &input, const std::string &prefix)
{
	std::size_t startPos = input.find(prefix);
	if (startPos != std::string::npos)
	{
		startPos += prefix.length() + 1;
		std::size_t endPos = input.find('\n', startPos) - 1;
		if (endPos == std::string::npos)
		{
			endPos = input.length();
		}
		return input.substr(startPos, endPos - startPos);
	}
	return "";
}
int Server::registration(std::string buff, size_t index)
{
	std::ostringstream reSend;
	if (index == _clients.size() + 1)
	{
		std::cout << (int)index << " " << (int)_clients.size() + 1 << std::endl;
		Client client(buff);
		_clients.push_back(client);
	}

	if (!_password.compare(parseValue(buff, "PASS")))
	{
		reSend << ": 001 " << parseValue(buff, "NICK") << " : Welcome to the IRC server!\n";
		std::string message = reSend.str();
		ssize_t nbytes = send(_fds[index].fd, message.c_str(), message.length(), 0);
		if (nbytes == -1)
		{
			return -1;
		}
	}
	return 1;
}

int Server::handleData(std::string buff, size_t index)
{
	if (registration(buff, index))
		return (0);
	return (0);
}

void Server::receivedData(size_t index)
{
	char buffer[1024];

	int nbytes = recv(_fds[index].fd, buffer, 1024, 0);
	if (nbytes <= 0)
	{
		if (nbytes == 0)
			std::cout << "Connection closed" << std::endl;
		else
			std::cerr << "recv failed" << "\n";
		close(_fds[index].fd);
		_fds.erase(_fds.begin() + index);
		return;
	}
	buffer[nbytes] = '\0';
	if (!handleData(buffer, index))
		return;
}