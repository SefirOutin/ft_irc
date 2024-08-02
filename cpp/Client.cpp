#include "Client.hpp"

// Client::Client(const char *ip, int port):
//   _port(port),
//   _sockFd(-1),
//   _ip(ip)
// {
//   memset(&_sockAddr, 0, sizeof(_sockAddr));
//   _sockAddr.sin_family = AF_INET;
//   _sockAddr.sin_port = htons(_port);
// }
// Client::~Client()
// {
//   close(_sockFd);
//   close(_connection);
// }
// int Client::connectToServer()
// {
//   if (inet_pton(AF_INET, _ip, &_serverSockAddr.sin_addr) <= 0)
//   {
//     std::cerr << "Adresse IP invalide" << std::endl;
//     return EXIT_FAILURE;
//   }

//   _serverSockFd = socket(AF_INET, SOCK_STREAM, 0);
//   if (_serverSockFd < 0)
//   {
//     std::cerr << "Erreur lors de la création de la socket" << std::endl;
//     return EXIT_FAILURE;
//   }

//   if (connect(_serverSockFd, (struct sockaddr *)&_serverSockAddr, sizeof(_serverSockAddr)) < 0)
//   {
//     std::cerr << "Erreur lors de la connexion au serveur" << std::endl;
//     close(_serverSockFd);
//     return EXIT_FAILURE;
//   }

//   return 0;
// }

// std::string Client::listensMsg()
// {
//   char buffer[256];

//   int n = recv(_sockFd, buffer, sizeof(buffer) - 1, 0);
//   if (n < 0)
//   {
//     std::cerr << "Erreur lors de la réception du message" << std::endl;
//     close(_sockFd);
//     return (NULL);
//   }
//   else if (n == 0)
//   {
//     std::cout << "Serveur déconnecté" << std::endl;
//     close(_sockFd);
//     return NULL;
//   }
//   buffer[n] = '\0';
//   std::cout << "Received from server >>>> " << buffer;

//   return std::string(buffer);
// }

// int Client::sendMsg(const char *buffer)
// {
//   if (send(_sockFd, buffer, strlen(buffer), 0) < 0)
//   {
//     std::cerr << "Erreur lors de l'envoi du message" << std::endl;
//     return EXIT_FAILURE;
//   }
//   return 0;
// }