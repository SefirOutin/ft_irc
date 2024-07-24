/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 14:23:05 by soutin            #+#    #+#             */
/*   Updated: 2024/07/24 19:10:30 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

int	main()
{
	int			sockFd, connection;
	socklen_t	addrLen;
	sockaddr_in	sockAddr;

	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd < 0)
	{
		std::cerr << "socket error\n";
		return 1;
	}
	
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(7777);
	sockAddr.sin_addr.s_addr = INADDR_ANY; //?

	if (bind(sockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
	{
		std::cerr << "bind error\n";
		return 1;
	}
	if (listen(sockFd, SOMAXCONN) < 0)
	{
		std::cerr << "listen error\n";
		return 1;
	}
	std::cout << "Listens ...\n";
	char buffer[256];
	while (1)
	{
		addrLen = sizeof(sockAddr);
		connection = accept(sockFd, (struct sockaddr*)&sockAddr, &addrLen);
		if (connection < 0)
		{
			std::cerr << "accept error\n";
			continue;
		}
		std::cout << "Connection made with " << inet_ntoa(sockAddr.sin_addr) << "\n";
		int n = recv(connection, buffer, 255, 0);
		if (n < 0)
		{
			std::cerr << "Error receiving message from client\n";
            close(connection);
            continue;
		}
		else if (n == 0)
		{
			std::cout << "Client disconnected\n";
			close(connection);
			continue;
		}
		buffer[n] = '\0';
		std::cout << buffer << "\n";
		if (send(connection, ": 001 bilou : Welcome to the IRC server!\n", 42, 0) < 0)
		{
			std::cerr << "send error\n";
			continue;
		}
	}
	close(sockFd);
	close(connection);
	return 0;
}