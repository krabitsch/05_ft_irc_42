/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandsAuth.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvass <pvass@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 14:00:50 by krabitsc          #+#    #+#             */
/*   Updated: 2026/01/14 13:03:14 by pvass            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// commandsAuth.cpp	 covers the commands: PASS/NICK/USER/QUIT

#include "../includes/Server.hpp"

// PASS COMMAND: requires server password (./ircserv <port> <password>)
void	Server::passCommand(Client &client, const IrcCommand &cmd)
{
	if (_password.empty())
	{
		client.setHasPass(true);
		return ;
	}
	if (client.isRegistered())
	{
		this->sendNumeric(client.getFd(), 462, client.getNickname(), std::vector<std::string>(),
				"You are already connected and cannot handshake again"); // libera gives this: 462 ERR_ALREADYREGISTRED
		return ;
	}
	if (cmd.parameters.empty())
	{
		this->sendNumeric(client.getFd(), 461, "*", std::vector<std::string>(1, "PASS"),
				"Not enough parameters");  // libera gives this: 461 ERR_NEEDMOREPARAMS
		return ;
	}
	if (cmd.parameters.size() > 1)
	{
		this->sendNumeric(client.getFd(), 461, "*", std::vector<std::string>(1, "PASS"),
					"Too many parameters"); // libera ignores this: no perfect numeric for too many params -> reuse 461
		return ;
	}

	if (cmd.parameters[0] != this->_password)
	{
		this->sendNumeric(client.getFd(), 464, "*", std::vector<std::string>(),
					"Password incorrect"); // 464 ERR_PASSWDMISMATCH -> disconnect client
		std::cout << RED << "Client (fd = " << client.getFd() << ") Disconnected" << WHITE << std::endl;
		this->clearClient(client.getFd());
		return ;
	}

	client.setHasPass(true);
	client.setPassword(cmd.parameters[0]);
 	//this->sendNotice(client.getFd(), "*", "Password accepted"); // libera sends no notice 
	this->tryRegisterClient(client);
}

// NICK COMMAND helper functions (static, file scope functions)
static bool isNickSpecial(char c)
{
	const std::string specials = "[]\\`_^{}|-";
	return (specials.find(c) != std::string::npos);
}

static bool isNickChar(unsigned char c)
{
	if (std::isalnum(c))
		return (true);
	return (isNickSpecial(static_cast<char>(c)));
}

static bool isNickFirstChar(unsigned char c)
{
	if (std::isalpha(c))
		return (true);
	return (isNickSpecial(static_cast<char>(c)));
}

static bool isAscii(unsigned char c)
{
	return (c <= 0x7F);
}

void Server::broadcastNickChange(Client& client, const std::string& oldNick, const std::string& newNick)
{
	// minimal prefix; later you can build nick!user@host
	const std::string user = client.getUsername().empty() ? "unknown" : client.getUsername();
    const std::string host = _serverName/* client.getHostname() */;
	std::string prefix = oldNick + "!" + user + "@" + host;

	std::cout << prefix << std::endl;

	std::string msg = ":" + prefix + " NICK :" + newNick + "\r\n";
	
	std::cout << msg << std::endl;

	send(client.getFd(), msg.c_str(), msg.length(), 0);
	std::map<std::string, char>* channels = client.GetChannel();
	if (!channels){
		return ;
	}

	for (std::map<std::string, char>::iterator it = channels->begin(); it != channels->end(); it++)
		broadcastToChannel(it->first, msg, -1); // include self too (exceptFd == -1)
}


// NICK: sets the nickname of the user
void	Server::nickCommand(Client &client, const IrcCommand &cmd)
{
	// decision to treat it this way: must have received PASS before NICK/USER
	if (/* !this->_password.empty() && */ !client.hasPass())
	{
		this->sendNumeric(client.getFd(), 451, "*", std::vector<std::string>(),
					"Password required"); // 451 ERR_NOTREGISTERED (PASS is required before NICK/USER)
			return ;
	}
	
	// get target to be used in sendNumeric message(s)
	std::string target = client.getNickname().empty() ? "*" : client.getNickname();

	if (cmd.parameters.empty())
	{
		this->sendNumeric(client.getFd(), 431, target,
			std::vector<std::string>(), "No nickname given"); // libera gives this
		return ;
	}

	std::string newNick = cmd.parameters[0];
	
	if (newNick.size() > 16)
		newNick = newNick.substr(0, 16); // libera cuts long nicknames and keeps first 16 chars (no notice about this)

	if (client.hasNick() && client.getNickname() == newNick)
		return ;

	// ascii-only + some allowed special chars
	if (newNick.empty() || !isNickFirstChar((unsigned char)newNick[0]))
	{
		this->sendNumeric(client.getFd(), 432, target,
			std::vector<std::string>(1, newNick), "Erroneous nickname"); // libera gives this: 432 ERR_ERRONEUSNICKNAME
		return ;
	}
	for (size_t i = 0; i < newNick.size(); i++)
	{
		unsigned char ch = static_cast<unsigned char>(newNick[i]);
		if (!isAscii(ch) || !isNickChar(ch))
		{
			this->sendNumeric(client.getFd(), 432, target,
				std::vector<std::string>(1, newNick), "Erroneous nickname");  // libera gives this: 432 ERR_ERRONEUSNICKNAME
			return ;
		}
	}

	// check if nickname is in use already
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		Client* cl = this->_clients[i];
		if (!cl)
			continue ;
		if (cl->getNickname() == newNick && cl->getFd() != client.getFd())
		{
			this->sendNumeric(client.getFd(), 433, target,	
				std::vector<std::string>(),	"Nickname is already in use"); // 433 ERR_NICKNAMEINUSE
			return ;
		}
	}

	// change to new nickname and broadcast new nickname message to everyone who shares a channel with this client
	std::string oldNick = client.getNickname();
	client.setNickname(newNick);
  	//this->sendNotice(client.getFd(), newNick, "Your nickname is now set to " + newNick); // libera sends no notice 
	client.setHasNick(true);

	if (client.isRegistered() && !oldNick.empty() && oldNick != newNick)
		broadcastNickChange(client, oldNick, newNick);

	if (!client.isRegistered())
		this->tryRegisterClient(client);
}

// USER COMMAND helper functions (static, file scope functions)
static bool isValidUserChar(unsigned char c)
{
	if (!std::isprint(c) || c == ' ')
		return false;
	if (std::isalnum(c))
		return (true);
	return (c == '_' || c == '-' || c == '.');
}

static bool isValidUsername(const std::string& u)
{
	if (u.empty() || u.size() > 32) // pick max username length to be 32 (RFC doesn’t force one)
		return (false);
	for (size_t i = 0; i < u.size(); i++)
	{
		unsigned char c = static_cast<unsigned char>(u[i]);
		if (c > 0x7F) // ASCII only
			return (false);
		if (!isValidUserChar(c))
			return (false);
	}
	return true;
}

// USER // syntax: USER <username> <mode> <unused> :<realname>
// while nickname need to be unique, the username doesn't 
// libera also accepts (and finishes registration) with input like USER a b c realname 
// so the implementation below checks it receives 4 params, but only <username> is really stored and used
void	Server::userCommand(Client &client, const IrcCommand &cmd)
{
	if (/* !_password.empty()  && */ !client.hasPass())
	{
		sendNumeric(client.getFd(), 451, "*", std::vector<std::string>(),
					"Password required");
		return ;
	}

	if (client.isRegistered())
	{
		sendNumeric(client.getFd(), 462, client.getNickname().empty() ? "*" : client.getNickname(),
					std::vector<std::string>(), "You are already connected and cannot handshake again"); // libera gives this
		return ;
	}

	// need 4 params: username, mode, unused, realname
	if (cmd.parameters.size() < 4 || cmd.parameters[0].empty())
	{
		sendNumeric(client.getFd(), 461, "*", std::vector<std::string>(1, "USER"), // libera gives this
					"Not enough parameters");
		return ;
	}

	const std::string& username = cmd.parameters[0];
	if (!isValidUsername(username))
	{
		sendNumeric(client.getFd(), 461, "*", std::vector<std::string>(1, "USER"),
					"Your username is invalid. Please make sure that your username contains only alphanumeric characters"); // libera gives this
		return ;
	}

	client.setUsername(username);
	client.setHasUser(true);
	// sendNotice(client.getFd(), "*", "Username accepted"); // libera sends no NOTICE
	tryRegisterClient(client);
}

// QUIT
//Exits the server 
// can be simply QUIT or QUIT :goodbyeMessage
void Server::quitCommand(Client &client, const IrcCommand &cmd)
{
	const int fd = client.getFd();

	// Extract quit message (trailing is appended as the last parameter, ignoring any others (should there by any))
	std::string message;
	if (!cmd.parameters.empty())
		message = cmd.parameters.back();
	if (message.empty())
		message = "Client Quit";

	// Build prefix nick!user@host
	const std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
	const std::string user = client.getUsername().empty() ? "unknown" : client.getUsername();
	const std::string host = client.getHostname();
	const std::string prefix = nick + "!" + user + "@" + host;

	const std::string quitLine = ":" + prefix + " QUIT :" + message + "\r\n";

	std::map<std::string, char>* chans = client.GetChannel();
	if (chans)
	{
		for (std::map<std::string, char>::iterator it = chans->begin(); it != chans->end(); ++it)
		{
			broadcastToChannel(it->first, quitLine, fd); // broadcast to all channels the client is in (excluding the quitter)
		}
	}

	// Send ERROR - this is the standard way to tell client "server is closing connection"
    std::string errorMsg = "ERROR :Closing Link: " + nick + "[" + host + "] (" + message + ")\r\n";
    sendConstructedMsg(fd, errorMsg);

	std::cout << RED << "Client (fd = " << fd << ") Disconnected" << WHITE << std::endl;
	clearClient(fd);
}
