/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 18:02:49 by soutin            #+#    #+#             */
/*   Updated: 2024/08/13 18:02:50 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

// Check the password set by the client
// if the password is correct save the connexion
// else close the connexion
void PASS(std::map<int, Client> *clients, std::string arg, std::string pass, int clientFd)
{
	std::cout << arg << std::endl;
	
	if ((*clients)[clientFd].checkPass())
	{
		std::string buff = ": 462  :You may not reregister !\r\n";
		send(clientFd, buff.c_str(), buff.size(), 0);
	}
	else if (arg.empty())
	{
		std::string buff = ": 461 * :Not enough parameters.\r\n";
		send(clientFd, buff.c_str(), buff.size(), 0);
		
		clients->erase(clientFd);
	}
	else if (arg.compare(pass))
	{
		close(clientFd);
		clients->erase(clientFd);
	}
	else
		(*clients)[clientFd].setPass(true);
}

void NICK(std::map<int, Client> *clients, std::string arg, int clientFd)
{
	if (arg.empty())
	{
		std::string buff = ": 431 * :No nickname given.\r\n";
		send(clientFd, buff.c_str(), buff.size(), 0);
		clients->erase(clientFd);
	}
	else if (nickAlreadyInUse(clients, arg, clientFd))
	{
		std::cout << arg << std::endl;

		std::string buff = ": 433 " + arg + " :" + arg + " is already in use\r\n";
		std::cout << buff;
		send(clientFd, buff.c_str(), buff.size(), 0);
		clients->erase(clientFd);
	}
	else if (!(*clients)[clientFd].checkPass())
	{
		std::string buff = ": 451 * :You have not registered!\r\n";
		send(clientFd, buff.c_str(), buff.size(), 0);
		clients->erase(clientFd);
	}
	else if (!verify_string_format(arg))
	{
		std::string buff = ": 432 * :Bad format on nickname\r\n";
		send(clientFd, buff.c_str(), buff.size(), 0);
		clients->erase(clientFd);
	}
	else if (arg != (*clients)[clientFd].getNick())
	{
		if (!(*clients)[clientFd].getNick().empty())
		{
			std::string buff = ":" + (*clients)[clientFd].getNick() + " NICK " + arg + "\r\n";
			send(clientFd, buff.c_str(), buff.size(), 0);
		}
		(*clients)[clientFd].setNick(arg);
		std::cout << "ok frere" << std::endl;
	}
}

void USER(std::map<int, Client> *clients, std::string arg, int clientFd)
{
	(*clients)[clientFd].setUser(arg);
	if ((*clients)[clientFd].checkPass() && !(*clients)[clientFd].getNick().empty())
	{
		(*clients)[clientFd].setUser(arg);
		std::string buff = ": 001 " + (*clients)[clientFd].getNick() + " :Welcome to the IRC server!\r\n";
		send(clientFd, buff.c_str(), buff.size(), 0);
	}
}