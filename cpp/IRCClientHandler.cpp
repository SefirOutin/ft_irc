/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClientHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soutin <soutin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 18:02:46 by soutin            #+#    #+#             */
/*   Updated: 2024/08/21 21:45:40 by soutin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCClientHandler.hpp"

IRCClientHandler::IRCClientHandler(int fd):
  _fd(fd)
{
  _pass = false;
}

IRCClientHandler::~IRCClientHandler() {}

const std::string IRCClientHandler::getNick() const
{
  return _nick;
}

bool IRCClientHandler::checkPass()
{
  return _pass;
}

const std::string IRCClientHandler::getUser() const
{
  return _user;
}

int IRCClientHandler::getFd() const
{
  return _fd;
}

void IRCClientHandler::setNick(std::string nick)
{
  _nick = nick;
}
void IRCClientHandler::setPass(bool pass)
{
  _pass = pass;
}
void IRCClientHandler::setUser(std::string user)
{
  _user = user;
}