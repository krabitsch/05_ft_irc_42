/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:59:44 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/25 13:42:17 by aruckenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

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

void 	Client::SetChannel(std::map<std::string, char> *newchannels)
{
    _channels = *newchannels;
}

void Client::SetNickname(std::string newname)
{
	_nickname = newname;
}

void Client::SetCurrentChannel(std::string newchannel)
{
    _currentChannel = newchannel;
}
// Public member functions/ methods


//Accepting New Client
//Accepts client in the server if they dont already exist 

//Setters/Getters and an insert function
void Client::addNofitication(std::string msg, char type)
{
    //_notifications.insert({type, msg});
}

void Client::AddChannel(std::string channelname)
{
   // _channels.insert({channelname, 'm'});    
}
