/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 18:02:46 by soutin            #+#    #+#             */
/*   Updated: 2024/08/13 18:02:47 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.class.hpp"

Client::Client()
{
  _pass = false;
}

Client::~Client() {}

std::string Client::getNick()
{
  return _nick;
}

bool Client::checkPass()
{
  return _pass;
}

std::string Client::getUser()
{
  return _user;
}

void Client::setNick(std::string nick)
{
  _nick = nick;
}
void Client::setPass(bool pass)
{
  _pass = pass;
}
void Client::setUser(std::string user)
{
  _user = user;
}