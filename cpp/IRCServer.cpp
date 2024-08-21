/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 19:34:36 by soutin            #+#    #+#             */
/*   Updated: 2024/08/21 14:16:52 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"

IRCServer::IRCServer(int port, const std::string &password)
		: _password(password)
{
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_port = htons(port);
	_sockAddr.sin_addr.s_addr = INADDR_ANY;
	_cmds["PASS"] = new PassCommand();
	_cmds["NICK"] = new NickCommand();
	_cmds["USER"] = new UserCommand();
}

IRCServer::~IRCServer()
{
	for (size_t i = 0; i < _fds.size(); i++)
		close(_fds[i].fd);
}

const std::string	IRCServer::getPass() const
{
	return (_password);
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
					acceptConnections();
				else
					receivedData(_fds[i].fd);
			}
		}
	}
	return (0);
}

bool IRCServer::nickAlreadyInUse(std::string arg, int clientFd)
{
	std::map<int, IRCClientHandler>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (!it->second.getNick().compare(arg) && it->first != clientFd)
			return (true);
	}
	return (false);
}
// A CHANGER PAR BILEL
void IRCServer::parseCommand(const std::string& buffer, int clientFd)
{
    std::stringstream ss(buffer);
    std::string command;
    std::getline(ss, command, ' ');

    std::map<std::string, IRCCommandHandler *>::iterator cmdIt = _cmds.find(command);
    if (cmdIt != _cmds.end())
    {
        std::string arg;
        std::getline(ss, arg);

        // Check if clientFd exists in _clients map
        std::map<int, IRCClientHandler>::iterator clientIt = _clients.find(clientFd);
        if (clientIt != _clients.end())
        {
            // Safely access the client handler and pass it to execute
            cmdIt->second->execute(*this, arg, clientIt->second);
        }
        else
        {
            std::cerr << "Client not found: " << clientFd << std::endl;
        }
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
    }
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

	// _clients[new_connection] = new IRCClientHandler(new_connection);
	IRCClientHandler	cli(new_connection);
	_clients.insert(std::pair<int, IRCClientHandler>(new_connection, cli));
	std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
	return (0);
}

void	IRCServer::closeConnection(int clientFd)
{
	std::map<int, IRCClientHandler>::iterator mapIt = _clients.find(clientFd);
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
	// if (!getCmd(buffer, clientFd))
	// 	return;
	parseCommand(buffer, clientFd);
}
// int IRCServer::getCmd(std::string buff, int clientFd)
// {
// 	std::string line;
// 	std::stringstream buffer(buff);

// 	int nLine = std::count(buff.begin(), buff.end(), '\n');
// 	while (nLine--)
// 	{
// 		if (std::getline(buffer, line))
// 		{
// 			std::string arg;
// 			size_t posLastCr = line.find("\r");
// 			size_t posFirstSpace = line.find(" ");
// 			std::string cmd = line.substr(0, posFirstSpace);
// 			if (posFirstSpace != arg.npos)
// 			{
// 				arg = line.substr(posFirstSpace + 1, posLastCr);
// 				arg.erase(std::remove(arg.begin(), arg.end(), '\r'), arg.end());
// 			}
// 			// std::cout << cmd << " " << arg << std::endl;
// 			handleData(cmd, arg, clientFd);
// 		}
// 	}
// 	return (0);
// }
// int IRCServer::handleData(std::string cmd, std::string arg, int clientFd)
// {
// 	if (_clients.find(clientFd) == _clients.end())
// 	{
// 		IRCClientHandler cli;
// 		_clients.insert(std::pair<int, IRCClientHandler>(clientFd, cli));
// 	}
// 	if (!cmd.compare("PASS"))
// 		PASS(&_clients, arg, _password, clientFd);
// 	else if (!cmd.compare("NICK"))
// 		NICK(&_clients, arg, clientFd);
// 	else if (!cmd.compare("USER"))
// 		USER(&_clients, arg, clientFd);
// 	// else if (!cmd.compare("JOIN"))
// 	// JOIN(arg, index);
// 	return (0);
// }


