  #include "../includes/Server.hpp"
  
#include "../includes/Server.hpp"
#include "../includes/Replies.hpp"
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstring>

  // Commands_Msg.cpp      // PRIVMSG/NOTICE

  //Privmsg 
  //Step 1:Get the username or nickname, look through it from the server, if they dont exist display a message carry on if they do
 

  /*void Server::privateMsg(std::string username, std::string msg)
  {
    std::cout << "Executing PRIVMSG to " << username << " with message: " << msg << std::endl;
    Client *client = findClient(-1, username);
    if (client == NULL)
    {
      this->sendNumeric(-1, 401, username, std::vector<std::string>(), "No such nick/channel");
      return ;
    }

    //send the msg to the user im not really sure how to do th at XD
    this->sendMessage(client->getFd(), "", "PRIVMSG", std::vector<std::string>(1, username), msg);

    //Old Message
		ssize_t sent = send(client->getFd(), msg.c_str(), msg.size(), 0);
		if (sent == -1) 
		{
		  std::cerr << "send() error on fd " << client->getFd() << ": " << std::strerror(errno) << std::endl;
		}
  }*/
 // PRIVMSG - Send a message to a user or channel

std::string Server::makePrivmsg(const std::string &prefix, const std::string &target, const std::string &msg)
{
    std::string line;
	line = ":" + prefix + " PRIVMSG " + target + " :" + msg + "\r\n";
    return line;
}

void Server::privateMsg(int senderFd, std::string target, std::string msg)
{
    // Find sender client
    Client *sender = findClient(senderFd, "");
    if (!sender)
        return;

	if (target.empty())
	{
		//411 ERR_NORECIPIENT
		sendNumeric(senderFd, 411, this->findClient(senderFd)->getNickname(), std::vector<std::string>(),
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
			this->sendNumeric(senderFd, 403, this->findClient(senderFd)->getNickname() , std::vector<std::string>(1, target),
					"No such channel");
			return ;
        }
        // Check if sender is on the channel
        if (!chan->isMember(sender))
        {
            // 442 ERR_NOTONCHANNEL
			this->sendNumeric(senderFd, 442, this->findClient(senderFd)->getNickname() , std::vector<std::string>(1, target),
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
        Client *recipient = findClient(-1, target);
        if (!recipient)
        {
            // ERR_NOSUCHNICK
			this->sendNumeric(senderFd, 401, this->findClient(senderFd)->getNickname() , std::vector<std::string>(1, target),
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

  //Notice
  //Idk what that is ask someone, maybe the client check if they received any request or notifications 
  void notice()
  {
    
  }