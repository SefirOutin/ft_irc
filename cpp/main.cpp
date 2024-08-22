/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 16:09:02 by soutin            #+#    #+#             */
/*   Updated: 2024/08/22 14:17:23 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "Usage: ./ircserv [port] [password]\n";
		return (1);
	}
	
	IRCServer	server(atoi(av[1]), av[2]);
	server.startServer();
	server.run();
	return (0);
}