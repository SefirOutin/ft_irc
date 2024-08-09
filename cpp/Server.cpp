/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 19:34:36 by soutin            #+#    #+#             */
/*   Updated: 2024/08/09 21:23:22 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
	std::cout << "accept fd: " << new_connection << "\n";
	clientPollFd.fd = new_connection;
	clientPollFd.events = POLLIN;
	_fds.push_back(clientPollFd);
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
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
		}
		else
			std::cerr << "recv failed"
								<< "\n";
		if (_clients[index].getFd() == _fds[index].fd)
			_clients.erase(_clients.begin() + index - 1);
		// if (_clients.size() == index)
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
int Server::getCmd(std::string buff, size_t index)
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
			handleData(cmd, arg, index);
		}
	}
	return 0;
}
int Server::handleData(std::string cmd, std::string arg, size_t index)
{
	if (_clients.size() < index)
	{
		Client cli;
		cli.setFd(_fds[index].fd);
		_clients.push_back(cli);
	}
	if (!cmd.compare("PASS"))
		PASS(arg, index);
	else if (!cmd.compare("NICK"))
		NICK(arg, index);
	else if (!cmd.compare("USER"))
		USER(arg, index);
	// else if (!cmd.compare("JOIN"))
	// JOIN(arg, index);
	return (0);
}

void Server::PASS(std::string arg, size_t index)
{
	if (!_clients[index - 1].getPass().empty())
	{
		std::string buff = ": 462  :You may not reregister !\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
	}
	else if (arg.compare(_password) && !arg.empty())
	{
		std::string buff = ": 464 * :Password incorrect !\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
		_clients.erase(_clients.begin() + (index - 1));
	}
	else if (!arg.compare(_password))
		_clients[index - 1].setPass(arg);
	else if (arg.empty())
	{
		std::string buff = ": 461 * :Not enough parameters.\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
		_clients.erase(_clients.begin() + (index - 1));
	}
}
bool verify_string_format(const std::string &input_string)
{
	for (size_t i = 0; i < input_string.size(); i++)
	{
		char c = input_string[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
					(c >= '0' && c <= '9') ||
					std::string("-[]\\`^{}").find(c) != std::string::npos))
			return false;
	}
	return true;
}
bool Server::nickAlreadyInUse(std::string arg, size_t index)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (!_clients[i].getNick().compare(arg) && i != index - 1)
			return (true);
	}
	return (false);
}
void Server::NICK(std::string arg, size_t index)
{

	if (arg.empty())
	{
		std::string buff = ": 431 * :No nickname given.\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
		_clients.erase(_clients.begin() + (index - 1));
	}
	else if (nickAlreadyInUse(arg, index))
	{
		std::cout << arg << std::endl;
		std::cout << index << std::endl;

		std::string buff = ": 433 " + arg + " :" + arg + " is already in use\r\n";
		std::cout << buff;
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
		_clients.erase(_clients.begin() + (index - 1));
	}
	else if (_clients[index - 1].getPass().empty())
	{
		std::cout << arg << std::endl;

		std::cout << index << std::endl;
		std::string buff = ": 451 * :You have not registered!\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
		_clients.erase(_clients.begin() + (index - 1));
	}
	else if (!verify_string_format(arg))
	{
		std::string buff = ": 432 * :Bad format on nickname\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
		_clients.erase(_clients.begin() + (index - 1));
	}
	else if (arg != _clients[index - 1].getNick())
	{
		if (!_clients[index - 1].getNick().empty())
		{
			std::string buff = ":" + _clients[index - 1].getNick() + " NICK " + arg + "\r\n";
			send(_fds[index].fd, buff.c_str(), buff.size(), 0);
		}
		_clients[index - 1].setNick(arg);
	}
}

void Server::USER(std::string arg, size_t index)
{
	_clients[index - 1].setUser(arg);
	if (!_clients[index - 1].getPass().compare(_password) && !_clients[index - 1].getNick().empty())
	{
		_clients[index - 1].setUser(arg);
		std::string buff = ": 001 " + _clients[index - 1].getNick() + " :Welcome to the IRC server!\r\n";
		send(_fds[index].fd, buff.c_str(), buff.size(), 0);
	}
}
