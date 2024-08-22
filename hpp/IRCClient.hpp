#ifndef IRCCLIENT_HPP
#define IRCCLIENT_HPP

#include "IRCServer.hpp"

class IRCClient : public IRCServer
{
public:
  IRCClient();
  IRCClient(int fd);
  ~IRCClient();

  const std::string &getNick() const;
  const std::string &getUser() const;
  int getFd() const;

  bool checkPass() const;
  void sendMessage(const std::string &msg) const;
  void receiveMessages();
  void setNick(std::string nick);
  void setUser(std::string user);
  void setPass(bool status);

private:
  int _fd;
  bool _pass;
  std::string _nick, _user;
};

#endif