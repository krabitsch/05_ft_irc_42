#include "../includes/Server.hpp"

void Server::joinCommand(int fd, const IrcCommand &cmd)
{
	if (!cmd.parameters.empty())
	{
		if (cmd.parameters[0][0] != '#')
		{
			// 403 ERR_NOSUCHCHANNEL
			this->sendNumeric(fd, 403, this->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, cmd.parameters[0]),
							"No such channel");
			return ;
		}
		if (cmd.parameters.size() == 2)
			join(fd, cmd.parameters[0], cmd.parameters[1]);
		else
			join(fd, cmd.parameters[0], "");
		DBG({std::cout << "User has joined channel: " << _channels[0].getname() << std::endl;});
	}
	else 
	{
		// 461 ERR_NEEDMOREPARAMS
		this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "JOIN"),
						"Not enough parameters");
	}
}

void Server::partCommand(int fd, const IrcCommand &cmd)
{
	if (!cmd.parameters.empty())
		part(fd, cmd.parameters[0]);
	else
	{	// 461 ERR_NEEDMOREPARAMS
		this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "PART"),"Not enough parameters");
	}
}

void Server::topicCommand(int fd, const IrcCommand &cmd)
{
	if (!cmd.parameters.empty() && cmd.parameters.size() == 2)
		topic(cmd.parameters[0], cmd.parameters[1], fd);
	else if (!cmd.parameters.empty() && cmd.parameters.size() == 1)
		topic(cmd.parameters[0], "", fd);
	else
		this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "TOPIC"),
			"Not enough parameters");
}

void Server::kickCommand(int fd, const IrcCommand &cmd)
{
	if (!cmd.parameters.empty())
	{
		if (cmd.parameters.size() < 2)
		{
			//ERR_NEEDMOREPARAMS 461
			this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "KICK"),
					"Not enough parameters");
			return ;
		}
		
		Channel *channel = findChannel(cmd.parameters[0]);			
		if (channel)
		{
			if (cmd.parameters.size() == 2)
				channel->kick(cmd.parameters[1], "", fd); //If no comments are included
			else if (cmd.parameters.size() == 3)
				channel->kick(cmd.parameters[1], cmd.parameters[2], fd); //if comments are included
		}
		else
		{
			//ERR_NOSUCHCHANNEL
			this->sendNumeric(fd, 403, findClientByFd(fd)->getNickname(), std::vector<std::string>(1, "KICK"),cmd.parameters[0] + " :No such channel");
		}
	}
	else 
	{
		//ERR_NEEDMOREPARAMS 461
		this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "KICK"),
						"Not enough parameters");
	}
}

void Server::modeCommand(int fd, const IrcCommand &cmd)
{
	if (!cmd.parameters.empty())
	{
		Channel* channel = findChannel(cmd.parameters[0]);
		if (channel == NULL)
		{
			//ERR_NOSUCHCHANNEL 403
			this->sendNumeric(fd, 403, findClientByFd(fd)->getNickname(), std::vector<std::string>(1, "MODE"),cmd.parameters[0] + " :No such channel");
		}
		else if (cmd.parameters.size() == 1)
			channel->mode(fd, "", ""); //If no mode parameters are given
		else
		{
			if (cmd.parameters.size() == 2)
				channel->mode(fd, cmd.parameters[1], "");
			else if (cmd.parameters.size() == 3)
				channel->mode(fd, cmd.parameters[1], cmd.parameters[2]);
		}
	}
	else 
	{
		//ERR_NEEDMOREPARAMS 461
		this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "MODE"),
					"Not enough parameters");
	}
}

void Server::inviteCommand(int fd, const IrcCommand &cmd)
{
	if (cmd.parameters.empty())
	{
		//ERR_NEEDMOREPARAMS
		this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "INVITE"),
					"Not enough parameters");
		return ;
	}
	Channel* channel = findChannel(cmd.parameters[0]); //Check if they create a new channel if one doesnt exist
	if (channel == NULL)
	{
		//ERR_NOSUCHCHANNEL 403
		this->sendNumeric(fd, 403, findClientByFd(fd)->getNickname(), std::vector<std::string>(1, "INVITE"),cmd.parameters[0] + " :No such channel");
		return ;
	}
	channel->invite(cmd.parameters[1], fd);
}