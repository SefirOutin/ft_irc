#include "../hpp/irc.hpp"

int	main(int ac, char **av)
{
	int	sockFd, connection;
	socklen_t addrLen;
  	sockaddr_in sockAddr;

  	sockFd = socket(AF_INET, SOCK_STREAM, 0);
  	if (sockFd < 0)
  	{
  	  std::cerr << "socket error\n";
  	  return 1;
  	}

  	sockAddr.sin_family = AF_INET;
  	sockAddr.sin_port = htons(4444);
  	sockAddr.sin_addr.s_addr = INADDR_ANY; //?

  	if (bind(sockFd, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0)
  	{
  	  std::cerr << "bind error\n";
  	  return 1;
  	}
  	if (listen(sockFd, SOMAXCONN) < 0)
  	{
  	  std::cerr << "listen error\n";
  	  return 1;
  	}
  	std::cout << "Listens ...\n";
  	while (connection <= 0)
  	{
  	  addrLen = sizeof(sockAddr);
  	  connection = accept(sockFd, (struct sockaddr *)&sockAddr, &addrLen);
  	  if (connection < 0)
  	  {
  	    std::cerr << "accept error\n";
  	    continue;
  	  }
  	  std::cout << "Connection made with " << inet_ntoa(sockAddr.sin_addr);
  	}
  	return 0;
}