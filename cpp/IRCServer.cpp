#include "IRCServer.hpp"

IRCServer::IRCServer(int port, const std::string &pass) :
	_running(false),
	_password(pass)
{
	createServerSocket(port);
}

IRCServer::~IRCServer()
{
	close(_serverSocket);
}

void IRCServer::start()
{
	int	pollCount;

	_running = true;
	while (_running)
	{
		pollCount = poll(_fds.data(), _fds.size(), -1);
		if (pollCount < 0)
		{
			std::cerr << "Poll failed" << std::endl;
			break;
		}
		for (size_t i = 0; i < _fds.size(); ++i)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _serverSocket)
				{
					acceptNewClient();
				}
				else
					_clients[_fds[i].fd]->start();
			}
		}
	}
}

void IRCServer::stop()
{
	_running = false;
}

void IRCServer::createServerSocket(int port)
{
	struct pollfd		pollFd;
	int 				on = 1;
	struct sockaddr_in sockAddr;
	
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		std::cerr << "socket error\n";
		exit(-1);
	}
	pollFd.fd = _serverSocket;
	pollFd.events = POLLIN;
	_fds.push_back(pollFd);
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on)) < 0)
	{
		std::cerr << "setsockopt() failed\n";
		exit(-1);
	}
	if (ioctl(_serverSocket, FIONBIO, (char *)&on) < 0)
	{
		std::cerr << "ioctl() failed\n";
		exit(-1);
	}
	bzero(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_serverSocket, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0)
	{
		std::cerr << "bind error\n";
		exit(-1);
	}
	if (listen(_serverSocket, SOMAXCONN) < 0)
	{
		std::cerr << "listen error\n";
		exit(-1);
	}
	std::cout << "Server started on port " << port << std::endl;
}

void IRCServer::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int newsockfd = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);

	if (newsockfd < 0)
	{
		std::cerr << "Error on accept" << std::endl;
		return;
	}

	pollfd clientPollfd;
	clientPollfd.fd = newsockfd;
	clientPollfd.events = POLLIN;
	_fds.push_back(clientPollfd);

	IRCClientHandler *handler = new IRCClientHandler(newsockfd, *_commandParser);
	_clients[newsockfd] = handler;

	std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

void IRCServer::closeClientConnection(int clientFd)
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
