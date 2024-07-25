/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 14:23:05 by soutin            #+#    #+#             */
/*   Updated: 2024/07/25 13:44:50 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
		return 0;
	Server serv(atoi(argv[1]));
	serv.startServer();
	serv.listensMsg();
}