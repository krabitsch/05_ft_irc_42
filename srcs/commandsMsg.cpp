#include "../includes/Server.hpp"
  
#include "../includes/Server.hpp"
#include "../includes/Replies.hpp"
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstring>

// PRIVMSG command
void Server::privmsgCommand(Client &client, const IrcCommand &cmd){

	int fd = client.getFd();
	DBG({std::cout << "Handling PRIVMSG command" << std::endl;});
	if (cmd.parameters.empty() || (cmd.parameters.size() == 1 && cmd.has_trailing == true))
	{
		//411 ERR_NORECIPIENT
		sendNumeric(fd, 411, this->findClientByFd(fd)->getNickname(), std::vector<std::string>(),
				"No recipient given (PRIVMSG)");
	}
	else if (cmd.parameters.size() == 1)
	{
		//412 ERR_NOTEXTTOSEND
		sendNumeric(fd, 412, this->findClientByFd(fd)->getNickname(), std::vector<std::string>(),
			":No text to send");
		return;
	}
	else if (cmd.parameters.size() > 6)
	{
		//407 ERR_TOOMANYTARGETS
		sendNumeric(fd, 407, this->findClientByFd(fd)->getNickname(), std::vector<std::string>(),
			":Too many recipients. No message delivered");
	}
	else
	{
		for (size_t i = 0; i < cmd.parameters.size() - 1; i++)
		{
			std::string target = cmd.parameters[i];
			std::string msg = cmd.parameters[cmd.parameters.size() - 1];
			privateMsg(fd, target, msg);
		}
	}

	return;
}

std::string Server::makePrivmsg(const std::string &prefix, const std::string &target, const std::string &msg)
{
	std::string line;
	line = ":" + prefix + " PRIVMSG " + target + " :" + msg + "\r\n";
	return line;
}

void Server::privateMsg(int senderFd, std::string target, std::string msg)
{
	// Find sender client
	Client *sender = findClientByFd(senderFd);
	if (!sender)
		return;

	if (target.empty())
	{
		//411 ERR_NORECIPIENT
		sendNumeric(senderFd, 411, this->findClientByFd(senderFd)->getNickname(), std::vector<std::string>(),
				"No recipient given (PRIVMSG)");
		return;
	}
	
	// Check if target is a channel (starts with #)
	if (target[0] == '#')
	{
		// Channel PRIVMSG - broadcast to all members except sender
		Channel *chan = findChannel(target);
		if (!chan)
		{
			// 403 ERR_NOSUCHCHANNEL
			this->sendNumeric(senderFd, 403, this->findClientByFd(senderFd)->getNickname() , std::vector<std::string>(1, target),
					"No such channel");
			return ;
		}
		// Check if sender is on the channel
		if (!chan->isMember(sender))
		{
			// 442 ERR_NOTONCHANNEL
			this->sendNumeric(senderFd, 442, this->findClientByFd(senderFd)->getNickname() , std::vector<std::string>(1, target),
					"You are not on channel");
			return ;
		}

		// Build sender prefix: nick!user@host
		std::string senderPrefix = sender->getNickname() + "!" + sender->getUsername() + "@localhost";
		std::string privmsgLine = makePrivmsg(senderPrefix, target, msg);

		// Forward to every channel member except sender
		std::vector<Client *>* members = chan->getMembers();
		for (size_t i = 0; i < members->size(); i++)
		{
			Client* member = (*members)[i];
			if (!member)
				continue;

			if (member->getFd() != senderFd)
			{
				ssize_t sent = send(member->getFd(), privmsgLine.c_str(), privmsgLine.length(), 0);
				if (sent == -1)
					std::cerr << "send() error on fd " << member->getFd()
							<< ": " << std::strerror(errno) << std::endl;
			}
		}
	}
	else
	{
		// Private PRIVMSG to a nick
		Client *recipient = findClientByNickOrUser(-1, target);
		if (!recipient)
		{
			// ERR_NOSUCHNICK
				this->sendNumeric(senderFd, 401, this->findClientByFd(senderFd)->getNickname() , std::vector<std::string>(1, target),
					"No such nick/channel");
				return ;
		}

		// Build sender prefix: nick!user@host
		std::string senderPrefix = sender->getNickname() + "!" + sender->getUsername() + "@localhost";
		std::string privmsgLine = makePrivmsg(senderPrefix, target, msg);

        // Send message to recipient
        ssize_t sent = send(recipient->getFd(), privmsgLine.c_str(), privmsgLine.length(), 0);
        if (sent == -1)
            std::cerr << "send() error on fd " << recipient->getFd() << ": " << std::strerror(errno) << std::endl;
    }
}
