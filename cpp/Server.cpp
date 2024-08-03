/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 19:34:36 by soutin            #+#    #+#             */
/*   Updated: 2024/08/03 19:55:30 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, const std::string &password):
	_password(password)
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
	int	on = 1;

	_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockFd < 0)
	{
		std::cerr << "socket error\n";
		return (1);
	}
	if (setsockopt(_sockFd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
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
				// else
					// 
            }
        }
    }
    return (0);
}

int Server::acceptConnections()
{
	struct sockaddr_in	clientSockAddr;
	struct pollfd		clientPollFd;
	socklen_t			addrLen;
    int					new_connection;
	
    addrLen = sizeof(clientSockAddr);
    new_connection = accept(_sockFd, (struct sockaddr*)&clientSockAddr, &addrLen);
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