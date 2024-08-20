#include "IRCServer.hpp"
IRCServer::IRCServer(int port, const std::string &pass) : port(port),  running(false),_password(pass)
{
	serverSocket = createServerSocket();
}

IRCServer::~IRCServer()
{
	close(serverSocket);
}

void IRCServer::start()
{
	running = true;
	std::cout << "Server started on port " << port << std::endl;

	std::vector<pollfd> pollfds;
	pollfd serverPollfd;
	serverPollfd.fd = serverSocket;
	serverPollfd.events = POLLIN;
	pollfds.push_back(serverPollfd);

	while (running)
	{
		// Appel de poll pour surveiller les sockets
		int pollCount = poll(&pollfds[0], pollfds.size(), -1);

		if (pollCount < 0)
		{
			std::cerr << "Poll failed" << std::endl;
			break;
		}

		// Vérification des événements sur chaque socket
		for (size_t i = 0; i < pollfds.size(); ++i)
		{
			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == serverSocket)
				{
					// Nouveau client qui se connecte
					acceptNewClient(pollfds);
				}
				else
					clients[pollfds[i].fd]->start();
			}
		}
	}
}

void IRCServer::stop()
{
	running = false;
}

int IRCServer::createServerSocket()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cerr << "Error creating socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverAddr;
	bzero((char *)&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Error binding socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	listen(sockfd, 5);
	return sockfd;
}

void IRCServer::acceptNewClient(std::vector<pollfd> &pollfds)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int newsockfd = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);

	if (newsockfd < 0)
	{
		std::cerr << "Error on accept" << std::endl;
		return;
	}

	pollfd clientPollfd;
	clientPollfd.fd = newsockfd;
	clientPollfd.events = POLLIN;
	pollfds.push_back(clientPollfd);

	IRCClientHandler *handler = new IRCClientHandler(newsockfd, commandParser);
	clients[newsockfd] = handler;

	std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

void IRCServer::closeClientConnection(pollfd &pfd, std::vector<pollfd> &pollfds, size_t index)
{
	std::cout << "Client disconnected" << std::endl;

	close(pfd.fd);
	delete clients[pfd.fd];
	clients.erase(pfd.fd);

	// Supprimer le pollfd du vector
	pollfds.erase(pollfds.begin() + index);
}
