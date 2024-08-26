#ifndef IRCERROR_HPP
#define IRCERROR_HPP
#include <iostream>

#define ERR_NONICKNAMEGIVEN ": 431 * :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick) (": 432 " + std::string(nick) + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(nick) (": 433 * " + std::string(nick) + " :Nickname is already in use\r\n")
#define ERR_NOTREGISTERED ": 451 * :You have not registered\r\n"
#define ERR_ALREADYREGISTRED ": 462 :You may not reregister\r\n"
#define ERR_NEEDMOREPARAMS(cmd) (": 461 " + std::string(cmd) + " :Not enough parameters\r\n")

#endif