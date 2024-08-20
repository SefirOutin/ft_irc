#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <poll.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <sstream>
#include <algorithm>
#include "Server.class.hpp"

class	Client;

void	PASS(std::map<int, Client> *clients, std::string arg, std::string pass, int clientFd);
void	NICK(std::map<int, Client> *clients, std::string arg, int clientFd);
void	USER(std::map<int, Client> *clients, std::string arg, int clientFd);

bool	nickAlreadyInUse(std::map<int, Client> *clients, std::string arg,int clientFd);
bool	verify_string_format(const std::string &input_string);

#endif