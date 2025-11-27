/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:59:44 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/27 11:32:58 by aruckenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// Constructors:
Client::Client() {}
Client::Client(const Client &other): _fdClient(other._fdClient), 
    _ipClient(other._ipClient),
    _nickname(other._nickname),
    _hostname(other._hostname),
    _username(other._username),
    _password(other._password),
    _channels(other._channels),
    _currentChannel(other._currentChannel),
    _notifications(other._notifications)
{}

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
int	Client::getFd() const 
{ 
	return (this->_fdClient); 
}

std::string Client::getNickname(void) const
{
	return (_nickname);
}

std::string Client::getUsername(void) const
{
	return (_username);
}

std::string Client::getCurrentChannel(void) const
{
    return (_currentChannel);
}

std::map<std::string, char> *Client::GetChannel(void)
{
	return (&_channels);
}


// Setters
void	Client::setFd(int fd)					
{ 
	this->_fdClient = fd;
}

void	Client::setIpAdd(std::string ipAddr)	
{ 
	this->_ipClient = ipAddr; 
}

void 	Client::setChannel(std::map<std::string, char> *newchannels)
{
    _channels = *newchannels;
}

void Client::setNickname(std::string newname)
{
	_nickname = newname;
}

void Client::setUsername(std::string newname)
{
	_username = newname;
}

void Client::setCurrentChannel(std::string newchannel)
{
    _currentChannel = newchannel;
}
// Public member functions/ methods


//Accepting New Client
//Accepts client in the server if they dont already exist 

//Setters/Getters and an insert function
void Client::addNofitication(std::string msg, char type)
{
    _notifications.insert(std::make_pair(type, msg));
}
void Client::AddChannel(std::string channelname)
{
	_channels.insert(std::make_pair(channelname, 'm'));    
}
