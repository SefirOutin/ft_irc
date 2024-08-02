#include "Proxy.hpp"
#include "irc.hpp"

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
  close(_serverSockFd);
  // close(_connection);
}

int Proxy::startProxy()
{
    struct pollfd tmpPollFd;
	int	on = 1;

	_proxySockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_proxySockFd < 0)
	{
		std::cerr << "socket error\n";
		return 1;
	}
	if (setsockopt(_proxySockFd, SOL_SOCKET,  SO_REUSEADDR,
                  (char *)&on, sizeof(on)) < 0)
	{
		// perror("setsockopt() failed");
		close(_proxySockFd);
		exit(-1);
	}
	if (ioctl(_proxySockFd, FIONBIO, (char *)&on) < 0)
	{
		// perror("ioctl() failed");
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
    tmpPollFd.fd = _proxySockFd;
    tmpPollFd.events = POLLIN;
    connections.push_back(tmpPollFd);

	if (inet_pton(AF_INET, _destIp, &_serverSockAddr.sin_addr) <= 0)
	{
		std::cerr << "Adresse IP invalide" << std::endl;
		return EXIT_FAILURE;
	}
	_serverSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSockFd < 0)
	{
		std::cerr << "Erreur lors de la création de la socket" << std::endl;
		return EXIT_FAILURE;
	}
	if (connect(_serverSockFd, (struct sockaddr *)&_serverSockAddr, sizeof(_serverSockAddr)) < 0)
	{
		std::cerr << "Erreur lors de la connexion au serveur" << std::endl;
		close(_serverSockFd);
		return EXIT_FAILURE;
	}
	std::cout << "Connected to server\n";
	tmpPollFd.fd = _serverSockFd;
    tmpPollFd.events = POLLIN;
    connections.push_back(tmpPollFd);
	return (0);
}

int Proxy::run()
{
    int	new_connection;
	struct sockaddr_in clientSockAddr;
	socklen_t addrLen;
	struct pollfd clientPollFd;
	char	buffer[1024];


    while (1)
    {
		// std::cout << "--------------\n";
        int poll_count = poll(connections.data(), connections.size(), -1);
        if (poll_count < 0)
        {
            std::cerr << "poll error\n";
            return 1;
        }
		// std::cout << "**************\n";
        for (size_t i = 0; i < connections.size(); ++i)
		{
            // std::cout << "i: " << i << "revent: " << connections[i].revents << "\n";
			if (connections[i].revents & POLLIN)
            {
                if (connections[i].fd == _proxySockFd)
                {
					acceptConnections();
                }
                else
                {
					forwardData(i);
                }
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
    if ((new_connection = accept(_proxySockFd, (struct sockaddr*)&clientSockAddr, &addrLen)) < 0)
    {
        std::cerr << "accept failed" << "\n";
        return (1);
    }
    clientPollFd.fd = new_connection;
	clientPollFd.events = POLLIN;
	connections.push_back(clientPollFd);
    std::cout << "New client connected: " << inet_ntoa(clientSockAddr.sin_addr) << std::endl;
}

void Proxy::forwardData(size_t index)
{
	char	buffer[1024];

    // Handle data from client or IRC server
    int nbytes = recv(connections[index].fd, buffer, 1024, 0);
    if (nbytes <= 0)
	{
        if (nbytes == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cerr << "recv failed" << "\n";
        close(connections[index].fd);
		connections.erase(connections.begin() + index);
    }
    else
    {
        buffer[nbytes] = '\0';
        std::cout << "Received data: " << buffer << std::endl;
        // Forward data between client and IRC server
        if (connections[index].fd == _serverSockFd)
        {
            // Data from IRC server to clients
            for (size_t j = 1; j < connections.size(); j++)
            {
                if (connections[j].fd != _serverSockFd)
                    send(connections[j].fd, buffer, nbytes, 0);
            }
        }
        else
        {
            // Data from client to IRC server
            send(_serverSockFd, buffer, nbytes, 0);
        }
    }
}
