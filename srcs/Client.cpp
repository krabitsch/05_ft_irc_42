/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:59:44 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/24 14:59:45 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

// Constructors:
Client::Client() {}
Client::Client(const Client &other): _fdClient(other._fdClient), _ipClient(other._ipClient)  {}

// Destructor:
Client::~Client() {}

// Operator overloads
Client&	Client::operator=(const Client& other)
{
	if (this != &other)
	{
		this->_fdClient = other._fdClient;
		this->_ipClient = other._ipClient;
	}
	return (*this);
}


// Getters
int		Client::getFd() const { return (this->_fdClient); }

// Setters
void	Client::setFd(int fd)					{ this->_fdClient = fd; }
void	Client::setIpAdd(std::string ipAddr)	{ this->_ipClient = ipAddr; }

// Public member functions/ methods


//Accepting New Client
//Accepts client in the server if they dont already exist 

//Setters/Getters and an insert function
void Client::addNofitication(std::string msg, char type)
{
    notifications.insert({type, msg});
}

void Client::SetChannel(std::map<std::string, char> *newchannels)
{
    channels = *newchannels;
}

void Client::AddChannel(std::string channelname)
{
    channels.insert({channelname, 'm'});    
}
void Client::SetNickname(std::string newname)
{
	nickname = newname;
}
std::string Client::GetNickname(void)
{
	return (nickname);
}
int Client::GetFdSocket(void)
{
	return (fd);
}

std::string Client::GetUsername(void)
{
	return (username);
}

std::string Client::GetCurrentChannel(void)
{
    return (current_channel);
}

void Client::SetCurrentChannel(std::string newchannel)
{
    current_channel = newchannel;
}

std::map<std::string, char> *Client::GetChannel(void)
{
	return (&channels);
}
