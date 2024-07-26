/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 14:23:05 by soutin            #+#    #+#             */
/*   Updated: 2024/07/26 20:45:01 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"
#include "Client.hpp"

int main(int argc, char **argv)
{
	if (argc != 4)
		return 0;
	Client client(argv[1], atoi(argv[2]));
	Server server(atoi(argv[3]));

	client.connectToServer();
	server.startSocket();
	server.connectToClient();
	while (1)
	{
		std::string msg = server.listensMsg();
		client.sendMsg(msg.c_str());
		msg = client.listensMsg();
		server.sendMsg(msg.c_str());
	}
}