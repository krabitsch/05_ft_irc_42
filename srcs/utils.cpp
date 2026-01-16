#include "../includes/Server.hpp"

void Server::broadcastMessage(int from_fd, const std::string& msg) 
{
	for (size_t i = 0; i < _clients.size(); ++i) 
	{
		int client_fd = _clients[i]->getFd();
		if (client_fd != from_fd) 
		{
			ssize_t sent = send(client_fd, msg.c_str(), msg.size(), 0);
			if (sent == -1) 
			{
				std::cerr << "send() error on fd " << client_fd << ": " << std::strerror(errno) << std::endl;
			}
		}
	}
}


void Server::broadcastMessage(const std::string &msgtype, const std::string& channelName, const std::string& nickname, const std::string& username, const std::string& message)
{
  std::string prefix = nickname + "!" + username + "@" + _serverName;
  std::string msg = ":" + prefix + " " + msgtype + " " + channelName + " :" + message + "\r\n";
  std::cout << "Broadcast massage: "<< msg << std::endl;
  broadcastToChannel(channelName, msg, -1);
}

void Server::broadcastToChannel(const std::string& channelName, const std::string& msg, int exceptFd)
{
	Channel* channel = findChannel(channelName);
	if (!channel)
		return ;

	std::vector<Client *>* members = channel->getMembers();
	if (!members)
		return ;

	// dbg print:
	std::cout << "broadcastToChannel: " << channelName << " members=" << members->size() << std::endl;
	for (size_t i = 0; i < members->size(); i++)
	{
		Client* m = (*members)[i];
		if (!m)
			continue ;

		int toFd = m->getFd();
		if (exceptFd != -1 && toFd == exceptFd) 
			continue ;
		std::cout << "  sending to fd " << toFd << ": " << msg << std::endl;
		sendConstructedMsg(toFd, msg); // msg ends with \r\n
	}
}

// ****************************************************************
// constructing message the server sends in the right format
// ****************************************************************

void	Server::sendConstructedMsg(int fd, const std::string &line)
{
	ssize_t sent = send(fd, line.c_str(), line.size(), 0);
	if (sent == -1)
		std::cerr << "send() error on fd " << fd << ": " << std::strerror(errno) << std::endl;
}

void	Server::sendMessage(int fd, const std::string &prefix, const std::string &command, 
							const std::vector<std::string> &params, const std::string &trailing)
{
	std::ostringstream oss; // read IRC message syntax into oss:
							// syntax: [:prefix] <command OR numeric> <params> :<optional text> (ends with \r\n)
	if (!prefix.empty())
		oss << ":" << prefix << " ";
	oss << command;
	for (size_t i = 0; i < params.size(); i++)
	{
		if (!params[i].empty())
			oss << " " << params[i];
	}

	if (!trailing.empty())
		oss << " :" << trailing;

	oss << "\r\n";

	std::string line = oss.str();
	this->sendConstructedMsg(fd, line);
}
// RFC1459: numeric reply must be sent as one message consisting of the sender prefix, the three digit numeric, and the target of the reply.
void	Server::sendNumeric(int fd, int code, const std::string &target, 
							const std::vector<std::string> &params, const std::string &trailing)
{
	std::ostringstream cmd;
	cmd.width(3);
	cmd.fill('0');
	cmd << code; // e.g. 1 -> "001"

	std::vector<std::string> fullParams;
	fullParams.push_back(target);
	for (size_t i = 0; i < params.size(); i++)
		fullParams.push_back(params[i]);

	this->sendMessage(fd, _serverName, cmd.str(), fullParams, trailing);
}

void	Server::sendNotice(int fd, const std::string &target, const std::string &text)
{
	std::vector<std::string> params;
	params.push_back(target);
	this->sendMessage(fd, _serverName, "NOTICE", params, text);
}

void	Server::sendWelcome(Client &client)
{
	const std::string &nick = client.getNickname();
	int fd = client.getFd();

	// 001 RPL_WELCOME
	sendNumeric(fd, 001, nick, std::vector<std::string>(),
		"Welcome to the ft_irc server " + nick);

	// 002 RPL_YOURHOST
	sendNumeric(fd, 002, nick, std::vector<std::string>(),
		"Your host is " + _serverName + ", running version 1.0");

	// 003 RPL_CREATED
	sendNumeric(fd, 003, nick, std::vector<std::string>(),
		"This server was created 2025-11-30");

	// 004 RPL_MYINFO
	// <servername> <version> <usermodes> <channelmodes>
	// supported user modes: i(nvisible)
	// supported channel modes (i invite-only, t topic protected, k key/password, o operator, l user limit)
	std::vector<std::string> params004;
	params004.push_back(_serverName);
	params004.push_back("ft_irc-42 1.0");
	params004.push_back("i");	   // user modes
	params004.push_back("itkol");  // channel modes
	sendNumeric(fd, 004, nick, params004, "");

	// 005 RPL_ISUPPORT
	std::vector<std::string> params005;
	params005.push_back("CHANTYPES=#");
	params005.push_back("NICKLEN=16");
	params005.push_back("USERLEN=32");
	params005.push_back("PREFIX=(o)@");
	params005.push_back("CASEMAPPING=ascii");
	sendNumeric(fd, 005, nick, params005, "are supported by this server");
}