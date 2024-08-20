/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 16:09:02 by soutin            #+#    #+#             */
/*   Updated: 2024/08/14 16:24:25 by soutin           ###   ########.fr       */
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
	server.start();
	// server.run();
	return (0);
}