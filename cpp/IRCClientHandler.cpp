
#include "IRCClientHandler.hpp"

IRCClientHandler::IRCClientHandler(int socket, IRCCommandParser &parser) : _clientSocket(socket), _commandParser(parser), _connected(true) {}

void IRCClientHandler::start()
{
  std::cout << "Client connected" << std::endl;
  receiveMessages();
}

void IRCClientHandler::stop()
{
  _connected = false;
  close(_clientSocket);
  std::cout << "Client disconnected" << std::endl;
}

void IRCClientHandler::sendMessage(const std::string &message)
{
  send(_clientSocket, message.c_str(), message.length(), 0);
}

void IRCClientHandler::setNick(const std::string &nick)
{
  this->_nick = nick;
}

std::string IRCClientHandler::getNick() const
{
  return _nick;
}

void IRCClientHandler::setUser(const std::string &userInfo)
{
  this->_user = userInfo;
}

void IRCClientHandler::receiveMessages()
{
  char buffer[512];
  while (_connected)
  {
    int bytesReceived = recv(_clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0)
    {
      buffer[bytesReceived] = '\0';
      std::string message(buffer);
      _commandParser.parseCommand(message, *this);
    }
    else
    {
      stop();
    }
  }
}
