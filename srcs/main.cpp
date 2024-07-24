/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 14:23:05 by soutin            #+#    #+#             */
/*   Updated: 2024/07/24 16:04:10 by soutin           ###   ########.fr       */
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
	sockAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
	{
		std::cerr << "bind error\n";
		return 1;
	}
	if (listen(sockFd, 5) < 0)
	{
		std::cerr << "lsiten error\n";
		return 1;
	}
	std::cout << "Listens ...\n";
	addrLen = sizeof(sockAddr);
	connection = accept(sockFd, (struct sockaddr*)&sockAddr, &addrLen);
	if (connection < 0)
	{
		std::cerr << "accept error\n";
		return 1;
	}
	std::cout << "Connection made.\n";
	return 0;
}