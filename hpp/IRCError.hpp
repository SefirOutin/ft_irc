#ifndef IRCERROR_HPP
#define IRCERROR_HPP

#define ERR_NOSUCHCHANNEL(nick, chanName) (": 403 " + std::string(nick) + " " + std::string(chanName) + ":No such channel\r\n")
#define	ERR_UNKNOWNCOMMAND(cmd) (": 421 " + (std::string(cmd) + " :Unknown command\r\n"))
#define ERR_NONICKNAMEGIVEN ": 431 * :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick) (": 432 " + std::string(nick) + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(nick) (": 433 * " + std::string(nick) + " :Nickname is already in use\r\n")
#define ERR_USERNOTINCHANNEL(nick, targetNick, chanName) (": 441 " + std::string(nick) + std::string(targetNick) + std::string(chanName) + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(nick, chanName) (": 442 " + std::string(nick) + " " + std::string(chanName) + " :You're not on that channel\r\n")
#define	ERR_USERONCHANNEL(nick, chanName) (": 443 " + std::string(nick) + " " + std::string(chanName) + " :is already on that channel\r\n")
#define ERR_NOTREGISTERED ": 451 * :You have not registered\r\n"
#define ERR_ALREADYREGISTRED ": 462 :You may not reregister\r\n"
#define ERR_NEEDMOREPARAMS(nick, cmd) (": 461 " + std::string(nick) + std::string(cmd) + " :Not enough parameters\r\n")
#define ERR_CHANNELISFULL(nick, chanName) (": 471 " + std::string(nick) + " " + std::string(chanName) + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHANNEL(nick, chanName) (": 473 " + std::string(nick) + " " + std::string(chanName) + " :Cannot join channel (+i)\r\n")
#define	ERR_BADCHANNELKEY(nick, chanName) (": 475 " + std::string(nick) + " " + std::string(chanName) + " :Cannot join channel (+k)\r\n")
#define	ERR_NOPRIVILEGES(nick) (": 481 " + std::string(nick) + " :Permission Denied- You're not an IRC operator\r\n")
#define	ERR_CHANOPRIVSNEEDED(nick, chanName) (": 482 " + std::string(nick) + " " + std::string(chanName) + " :You're not channel operator\r\n")
#define ERR_NOTEXTTOSEND(nick) (": 412 " + std::string(nick) + " :No text to send\r\n")
#define ERR_NORECIPIENT(nick, cmd) (": 411 " + std::string(nick) + " :No recipient given (" + std::string(cmd) + ")\r\n")
#define ERR_NOSUCHNICK(nick, nicktosend) (": 401 " + std::string(nick) + " " + std::string(nicktosend) + " :No such nick/channel\r\n")
#define ERR_UNKNOWNMODE(nick, mode) (": 472 " + std::string(nick) + " " + std::string(mode) + " :is unknown mode\r\n")
// #define ERR_CANNOTSENDTOCHAN(nick, chanName) (": 404 " + std::string(nick) + " " + std::string(chanName) + " :Cannot send to channel\r\n")

#define RPL_CHANNELMODEIS(nick, channel, modes) "324 " + std::string(nick) + " " + std::string(chanName) + " " + std::string(modes) + "\r\n"

#endif