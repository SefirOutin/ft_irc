#ifndef IRCCMDS_HPP
#define IRCCMDS_HPP

#include "IRCCommandHandler.hpp"
#include "IRCClient.hpp"

class NickCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class PassCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class UserCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class PingCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class CapCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class PrivmsgCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class JoinCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class PartCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class KickCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class InviteCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class TopicCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class ModeCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

class QuitCommand : public IRCCommandHandler
{
	public:
		void execute(const std::string &params, IRCClient &client);
};

#endif