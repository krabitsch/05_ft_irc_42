/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandsAuth.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 14:00:50 by krabitsc          #+#    #+#             */
/*   Updated: 2025/12/28 19:37:13 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// commandsAuth.cpp	 covers the commands: PASS/NICK/USER/QUIT

#include "../includes/Server.hpp"

// PASS COMMAND: requires server password (./ircserv <port> <password>)
void	Server::passCommand(Client &client, const IrcCommand &cmd)
{
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

	const std::string &passwordInput = cmd.parameters[0];
	if (passwordInput != this->_password)
	{
		this->sendNumeric(client.getFd(), 464, "*", std::vector<std::string>(),
					"Password incorrect"); // 464 ERR_PASSWDMISMATCH -> disconnect client
		this->clearClients(client.getFd());
		close(client.getFd());
		return ;
	}

	client.setHasPass(true);
	client.setPassword(passwordInput);
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
	std::string prefix = oldNick;

	std::string msg = ":" + prefix + " NICK :" + newNick + "\r\n";

	std::map<std::string, char>* channels = client.GetChannel();
	if (!channels)
		return ;

	for (std::map<std::string, char>::iterator it = channels->begin(); it != channels->end(); it++)
		broadcastToChannel(it->first, msg, -1); // include self too (exceptFd == -1)
}


// NICK: sets the nickname of the user
void	Server::nickCommand(Client &client, const IrcCommand &cmd)
{
	// decision to treat it this way: must have received PASS before NICK/USER
	if (!client.hasPass())
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
		if (this->_clients[i].getNickname() == newNick && this->_clients[i].getFd() != client.getFd())
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

	this->tryRegisterClient(client);

}

//User
void	Server::userCommand(Client &client, const IrcCommand &cmd) // syntax: USER <username> <mode> <unused> :<realname>
{
	// KR: need to still implement this, for now setHasUser = true, so registration can be completed and other commands teste
	(void)cmd;

	if (client.isRegistered())
	{
		// 462 ERR_ALREADYREGISTRED
		this->sendNumeric(client.getFd(), 462, client.getNickname(), std::vector<std::string>(),
					"You are already connected and cannot handshake again"); // libera gives this
		return ;
	}
	client.setHasUser(true);
  	this->sendNotice(client.getFd(), "*", "Username accepted");
	this->tryRegisterClient(client);
}

//Quit 
//Exits the server 

void Server::quitCommand(std::string message, int fd)
{
	//Just need to add the custom message feature! - Al
	(void)message;
	
	Client *client = findClientByFd(fd);
	if (client == NULL)
		return ;
	this->sendNotice(fd, "*", "You have quit the server. Goodbye!"); //Sends a notice to the client
	this->clearClients(fd); //Clears the client from the server
	std::cout << RED << "Client (fd = " << fd << ") Disconnected" << WHITE << std::endl;
	close(fd); //Closes the connection
	return ;
}