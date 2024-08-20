#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <poll.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <sstream>
#include <algorithm>
#include "IRCCommandParser.hpp"
#include "IRCClientHandler.hpp"

// class IRC

class IRCServer
{
	public:
		IRCServer(int port, const std::string &password);
		~IRCServer();

		int start();
		// int run();

	private:
		int 								_sockFd;
		// sockaddr_in					        _sockAddr;
		std::vector<struct pollfd>          _fds;
        IRCCommandParser                    _commandParser;
        std::map<int, IRCClientHandler*>    _clients;
		std::string							_password;
		
		void	socketOpt();
		int		acceptConnections();
		void	closeConnection(int clientFd);
		void	receivedData(int clientFd);
		int		getCmd(std::string buff, int clientFd);
		int		handleData(std::string cmd, std::string arg, int clientFd);
		void	createServerSocket(int port);

};

// Command handlers, IRCClientHandler, etc., devraient être définis ici

// class BasicIRCServer {
// public:
//     BasicIRCServer(int port) : port(port), running(false) {
//         serverSocket = createServerSocket();
//     }

//     ~BasicIRCServer() {
//         close(serverSocket);
//     }

//     void start() {
//         running = true;
//         std::cout << "IRCServer started on port " << port << std::endl;

//         std::vector<pollfd> pollfds;
//         pollfd serverPollfd;
//         serverPollfd.fd = serverSocket;
//         serverPollfd.events = POLLIN;
//         pollfds.push_back(serverPollfd);

//         while (running) {
//             // Appel de poll pour surveiller les sockets
//             int pollCount = poll(&pollfds[0], pollfds.size(), -1);

//             if (pollCount < 0) {
//                 std::cerr << "Poll failed" << std::endl;
//                 break;
//             }

//             // Vérification des événements sur chaque socket
//             for (size_t i = 0; i < pollfds.size(); ++i) {
//                 if (pollfds[i].revents & POLLIN) {
//                     if (pollfds[i].fd == serverSocket) {
//                         // Nouveau client qui se connecte
//                         acceptNewClient(pollfds);
//                     } else {
//                         // Données reçues d'un client existant
//                         handleClientMessage(pollfds[i], pollfds, i);
//                     }
//                 }
//             }
//         }
//     }

//     void stop() {
//         running = false;
//     }

// private:
//     int serverSocket;
//     int port;
//     bool running;
//     IRCCommandParser commandParser;
//     std::map<int, IRCClientHandler*> clients;

//     int createServerSocket() {
//         int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//         if (sockfd < 0) {
//             std::cerr << "Error creating socket" << std::endl;
//             exit(EXIT_FAILURE);
//         }

//         struct sockaddr_in serverAddr;
//         bzero((char *)&serverAddr, sizeof(serverAddr));
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_addr.s_addr = INADDR_ANY;
//         serverAddr.sin_port = htons(port);

//         if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
//             std::cerr << "Error binding socket" << std::endl;
//             exit(EXIT_FAILURE);
//         }

//         listen(sockfd, 5);
//         return sockfd;
//     }

//     void acceptNewClient(std::vector<pollfd>& pollfds) {
//         struct sockaddr_in clientAddr;
//         socklen_t clientLen = sizeof(clientAddr);
//         int newsockfd = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);

//         if (newsockfd < 0) {
//             std::cerr << "Error on accept" << std::endl;
//             return;
//         }

//         pollfd clientPollfd;
//         clientPollfd.fd = newsockfd;
//         clientPollfd.events = POLLIN;
//         pollfds.push_back(clientPollfd);

//         IRCClientHandler* handler = new IRCClientHandler(newsockfd, commandParser);
//         clients[newsockfd] = handler;

//         std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
//     }

//     void handleClientMessage(pollfd& pfd, std::vector<pollfd>& pollfds, size_t index) {
//         IRCClientHandler* client = clients[pfd.fd];
//         if (client == nullptr) return;

//         char buffer[512];
//         int n = read(pfd.fd, buffer, sizeof(buffer) - 1);

//         if (n <= 0) {
//             // Le client a fermé la connexion
//             closeClientConnection(pfd, pollfds, index);
//         } else {
//             buffer[n] = '\0';
//             client->receiveMessage(buffer);
//         }
//     }

//     void closeClientConnection(pollfd& pfd, std::vector<pollfd>& pollfds, size_t index) {
//         std::cout << "Client disconnected" << std::endl;

//         close(pfd.fd);
//         delete clients[pfd.fd];
//         clients.erase(pfd.fd);

//         // Supprimer le pollfd du vector
//         pollfds.erase(pollfds.begin() + index);
//     }
// };

#endif