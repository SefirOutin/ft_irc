#ifndef IRCCLIENT_HPP
#define IRCCLIENT_HPP
#include "IRCServer.hpp"
class IRCServer;
class IRCClient
{
public:
  IRCClient() {};
  IRCClient(int fd, IRCServer *server);
  ~IRCClient();

  const std::string &getNick() const;
  const std::string &getUser() const;
  int getFd() const;

  bool checkPass(const std::string &password) const;
  void sendMessage(const std::string &msg) const;
  void receiveMessages();
  void setNick(std::string nick);
  void setUser(std::string user);
  void setConnected(bool status);
  bool isConnected() const;
  bool nickAlreadyInUse(std::string arg, int clientFd);

private:
  IRCServer *_server;
  int _fd;
  bool _connected;
  std::string _nick, _user;
};

#endif