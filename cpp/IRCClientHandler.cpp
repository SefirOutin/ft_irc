/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClientHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 18:02:46 by soutin            #+#    #+#             */
/*   Updated: 2024/08/22 12:34:57 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCClientHandler.hpp"

IRCClientHandler::IRCClientHandler(int fd) : _fd(fd)
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

void IRCClientHandler::sendMessage(const std::string &msg)
{
  send(_fd, msg.c_str(), msg.length(), 0);
}