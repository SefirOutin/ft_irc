/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 14:23:05 by soutin            #+#    #+#             */
/*   Updated: 2024/08/28 16:39:01 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Proxy.hpp"

int main(int argc, char **argv)
{
	if (argc != 4)
		return (0);
	Proxy proxy(atoi(argv[1]), argv[2], atoi(argv[3]));
	proxy.startProxy();
	proxy.run();
}

// #include <iostream>
// #include <cstring>
// #include <cstdio>
// #include <cstdlib>
// #include <cerrno>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <poll.h>

// #define PORT 4444  // Proxy server port
// #define IRC_SERVER "127.0.0.1"  // Replace with actual IRC server
// #define IRC_PORT 7777  // IRC server port
// #define BUFFER_SIZE 1024
// #define MAX_CLIENTS 100

// int main() {
//     int proxy_fd, client_fd, irc_fd;
//     struct sockaddr_in proxy_addr, client_addr, irc_addr;
//     socklen_t addr_len;
//     struct pollfd fds[MAX_CLIENTS];
//     int nfds = 1;

//     // Create proxy socket
//     if ((proxy_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         perror("socket failed");
//         exit(EXIT_FAILURE);
//     }

//     // Set proxy server address
//     memset(&proxy_addr, 0, sizeof(proxy_addr));
//     proxy_addr.sin_family = AF_INET;
//     proxy_addr.sin_addr.s_addr = INADDR_ANY;
//     proxy_addr.sin_port = htons(PORT);

//     // Bind the socket to the port
//     if (bind(proxy_fd, (struct sockaddr*)&proxy_addr, sizeof(proxy_addr)) < 0) {
//         perror("bind failed");
//         close(proxy_fd);
//         exit(EXIT_FAILURE);
//     }

//     // Listen for incoming connections
//     if (listen(proxy_fd, 10) < 0) {
//         perror("listen failed");
//         close(proxy_fd);
//         exit(EXIT_FAILURE);
//     }

//     // Initialize pollfd structure
//     fds[0].fd = proxy_fd;
//     fds[0].events = POLLIN;

//     // Create IRC socket
//     if ((irc_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         perror("IRC socket failed");
//         close(proxy_fd);
//         exit(EXIT_FAILURE);
//     }

//     // Set IRC server address
//     memset(&irc_addr, 0, sizeof(irc_addr));
//     irc_addr.sin_family = AF_INET;
//     irc_addr.sin_port = htons(IRC_PORT);

//     if (inet_pton(AF_INET, IRC_SERVER, &irc_addr.sin_addr) <= 0) {
//         perror("inet_pton failed");
//         close(proxy_fd);
//         close(irc_fd);
//         exit(EXIT_FAILURE);
//     }

//     // Connect to IRC server
//     if (connect(irc_fd, (struct sockaddr*)&irc_addr, sizeof(irc_addr)) < 0) {
//         perror("connect to IRC server failed");
//         close(proxy_fd);
//         close(irc_fd);
//         exit(EXIT_FAILURE);
//     }

//     // Add IRC server to pollfd structure
//     fds[nfds].fd = irc_fd;
//     fds[nfds].events = POLLIN;
//     nfds++;

//     std::cout << "Proxy server is running on port " << PORT << std::endl;

//     char buffer[BUFFER_SIZE];
//     while (true)
//     {
//         int poll_count = poll(fds, nfds, -1);

//         if (poll_count < 0)
//         {
//             perror("poll failed");
//             break;
//         }

//         for (int i = 0; i < nfds; i++)
//         {
//             std::cout << "i: " << i << "revent: " << fds[i].revents << "\n";
//             if (fds[i].revents & POLLIN)
//             {
//                 if (fds[i].fd == proxy_fd)
//                 {
//                     // Accept new client connection
//                     addr_len = sizeof(client_addr);
//                     if ((client_fd = accept(proxy_fd, (struct sockaddr*)&client_addr, &addr_len)) < 0)
//                     {
//                         perror("accept failed");
//                         continue;
//                     }

//                     // Add new client to pollfd structure
//                     fds[nfds].fd = client_fd;
//                     fds[nfds].events = POLLIN;
//                     nfds++;
//                     std::cout << "New client connected" << std::endl;
//                 }
//                 else
//                 {
//                     // Handle data from client or IRC server
//                     int nbytes = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);

//                     if (nbytes <= 0) {
//                         if (nbytes == 0)
//                             std::cout << "Connection closed" << std::endl;
//                         else
//                             perror("recv failed");

//                         close(fds[i].fd);
//                         fds[i] = fds[--nfds];
//                     }
//                     else
//                     {
//                         buffer[nbytes] = '\0';
//                         std::cout << "Received data: " << buffer << std::endl;

//                         // Forward data between client and IRC server
//                         if (fds[i].fd == irc_fd)
//                         {
//                             // Data from IRC server to clients
//                             for (int j = 1; j < nfds; j++)
//                             {
//                                 if (fds[j].fd != irc_fd)
//                                     send(fds[j].fd, buffer, nbytes, 0);
//                             }
//                         }
//                         else
//                         {
//                             // Data from client to IRC server
//                             send(irc_fd, buffer, nbytes, 0);
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     close(proxy_fd);
//     close(irc_fd);
//     return 0;
// }