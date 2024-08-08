#pragma once

// #include <iostream>
// #include <map>
// #include <unistd.h>
// #include <string.h>
#include <string>
// #include <vector>
// #include <poll.h>
// #include <errno.h>
// #include <sys/ioctl.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <cstdlib>
// #include <ctime>
// #include <sys/time.h>
#include "Server.hpp"

class Client
{
public:
	Client();
	~Client();

	std::string getNick();
	std::string getPass();
	std::string getUser();
	void setNick(std::string nick);
	void setPass(std::string pass);
	void setUser(std::string user);

private:
	std::string _nick;
	std::string _pass;
	std::string _user;
};
