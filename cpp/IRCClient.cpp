#include "IRCClient.hpp"

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClient.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoudach <bmoudach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 18:02:46 by soutin            #+#    #+#             */
/*   Updated: 2024/08/22 13:43:20 by bmoudach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCClient.hpp"

IRCClient::IRCClient(int fd) : _fd(fd)
{
  _pass = false;
}

IRCClient::~IRCClient() {}

const std::string &IRCClient::getNick() const
{
  return _nick;
}

bool IRCClient::checkPass() const
{
  return _pass;
}

const std::string &IRCClient::getUser() const
{
  return _user;
}

int IRCClient::getFd() const
{
  return _fd;
}

void IRCClient::setNick(std::string nick)
{
  _nick = nick;
}
void IRCClient::setPass(bool pass)
{
  _pass = pass;
}
void IRCClient::setUser(std::string user)
{
  _user = user;
}

void IRCClient::sendMessage(const std::string &msg) const
{
  send(_fd, msg.c_str(), msg.length(), 0);
}

void IRCClient::receiveMessages()
{
  char buffer[512];
  {
    int bytesReceived = recv(_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0)
    {
      buffer[bytesReceived] = '\0';
      std::string message(buffer);
      parseCmds(message, *this);
    }
    else
    {
      close(_fd);
    }
  }
}