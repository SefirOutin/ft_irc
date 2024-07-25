#pragma once

#include <iostream>
#include <map>

class Client
{

public:
  Client(std::string _nick, std::string pass, std::string _user);
  ~Client();

private:
  std::string _nick;
  std::string _pass;
  std::string _user;
};
