/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Proxy.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 16:10:24 by soutin            #+#    #+#             */
/*   Updated: 2024/08/28 16:38:21 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Proxy.hpp"

Proxy::Proxy(int proxyPort, const char *destIp, const int destPort) :
  _destIp(destIp)
{
  memset(&_serverSockAddr, 0, sizeof(_serverSockAddr));
  _serverSockAddr.sin_family = AF_INET;
  _serverSockAddr.sin_port = htons(destPort);

  _proxySockAddr.sin_family = AF_INET;
  _proxySockAddr.sin_port = htons(proxyPort);
  _proxySockAddr.sin_addr.s_addr = INADDR_ANY;
}

Proxy::~Proxy()
{
  for (size_t i = 0; i < _fds.size(); i++)
    close(_fds[i].fd);
}

int Proxy::startProxy()
{
    struct pollfd proxyPollFd;
	int	on = 1;

	_proxySockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_proxySockFd < 0)
	{
		std::cerr << "socket error\n";
		return (1);
	}
	if (setsockopt(_proxySockFd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
	{
		std::cerr << "setsockopt() failed\n";
		close(_proxySockFd);
		exit(-1);
	}
	if (ioctl(_proxySockFd, FIONBIO, (char *)&on) < 0)
	{
		std::cerr << "ioctl() failed";
		close(_proxySockFd);
		exit(-1);
	}
	if (bind(_proxySockFd, (struct sockaddr *)&_proxySockAddr, sizeof(_proxySockAddr)) < 0)
	{
		std::cerr << "bind error\n";
		return 1;
	}
	if (listen(_proxySockFd, SOMAXCONN) < 0)
	{
		std::cerr << "listen error\n";
		return 1;
	}
	std::cout << "Listens ...\n";
    proxyPollFd.fd = _proxySockFd;
    proxyPollFd.events = POLLIN;
    _fds.push_back(proxyPollFd);
    connectToServer();
	return (0);
}

long getCurrentTimeInMilliseconds() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

int Proxy::run()
{
    while (1)
    {
        int poll_count = poll(_fds.data(), _fds.size(), -1);
        if (poll_count < 0)
        {
            std::cerr << "poll error\n";
            return 1;
        }
        for (size_t i = 0; i < _fds.size(); ++i)
		{
			if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _proxySockFd)
					acceptConnections();
                else
					forwardData(i);
            }
        }
    }
    return 0;
}

int Proxy::acceptConnections()
{
    int	new_connection;
	struct sockaddr_in clientSockAddr;
	socklen_t addrLen;
	struct pollfd clientPollFd;
	
    addrLen = sizeof(clientSockAddr);
    new_connection = accept(_proxySockFd, (struct sockaddr*)&clientSockAddr, &addrLen);
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

void Proxy::forwardData(size_t index)
{
	char	buffer[1024];

    int nbytes = recv(_fds[index].fd, buffer, 1024, 0);
    if (nbytes <= 0)
	{
        if (nbytes == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cerr << "recv failed" << "\n";
        close(_fds[index].fd);
		_fds.erase(_fds.begin() + index);
        return ;
    }
    buffer[nbytes] = '\0';
    if (_fds[index].fd == _serverSockFd)
    {
        for (size_t j = 2; j < _fds.size(); j++)
            send(_fds[j].fd, buffer, nbytes, 0);
        std::cout << "data to client: " << buffer << std::endl;
        
    }
    else
    {
        send(_serverSockFd, buffer, nbytes, 0);
        std::cout << "data to server: " << buffer << std::endl;
    }
}


int Proxy::connectToServer()
{  
    struct pollfd serverPollFd;
    
    if (inet_pton(AF_INET, _destIp, &_serverSockAddr.sin_addr) <= 0)
	{
		std::cerr << "Adresse IP invalide" << std::endl;
		return (EXIT_FAILURE);
	}
	_serverSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSockFd < 0)
	{
		std::cerr << "Erreur lors de la création de la socket" << std::endl;
		return (EXIT_FAILURE);
	}
	if (connect(_serverSockFd, (struct sockaddr *)&_serverSockAddr, sizeof(_serverSockAddr)) < 0)
	{
		std::cerr << "Erreur lors de la connexion au serveur" << std::endl;
		close(_serverSockFd);
		return (EXIT_FAILURE);
	}
	std::cout << "Connected to server\n";
	serverPollFd.fd = _serverSockFd;
    serverPollFd.events = POLLIN;
    _fds.push_back(serverPollFd);
    return (0);
}