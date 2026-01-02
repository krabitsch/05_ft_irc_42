/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:59:44 by krabitsc          #+#    #+#             */
/*   Updated: 2025/12/04 10:59:42 by aruckenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// Constructors:
Client::Client(): _server(NULL),
				  _fdClient(-1),
				  _ipClient(""),
				  _nickname(""),
				  _hostname(""),
				  _username("unknown"),
				  _password(""),
				  _channels(),
				  _currentChannel(""),
				  _notifications(),
				  _hasPass(false),
				  _hasNick(false),
				  _hasUser(false),
				  _registered(false)
{}

Client::Client(const Client &other): _server(other._server), 
									 _fdClient(other._fdClient),
									 _ipClient(other._ipClient),
									 _nickname(other._nickname),
									 _hostname(other._hostname),
									 _username(other._username),
									 _password(other._password),
									 _channels(other._channels),
									 _currentChannel(other._currentChannel),
									 _notifications(other._notifications),
									 _hasPass(other._hasPass),
									 _hasNick(other._hasNick),
									 _hasUser(other._hasUser),
									 _registered(other._registered)
{}

// Destructor:
Client::~Client() {}

// Operator overloads
Client&	Client::operator=(const Client& other)
{
	if (this != &other)
	{
		this->_server			= other._server;
		this->_fdClient			= other._fdClient;
		this->_ipClient			= other._ipClient;
		this->_nickname		 	= other._nickname;
		this->_hostname		 	= other._hostname;
		this->_username		 	= other._username;
		this->_password		 	= other._password;
		this->_channels		 	= other._channels;
		this->_currentChannel	= other._currentChannel;
		this->_notifications	= other._notifications;
		this->_hasPass		 	= other._hasPass;
		this->_hasNick		 	= other._hasNick;
		this->_hasUser		 	= other._hasUser;
		this->_registered	 	= other._registered;
	}
	return (*this);
}

// Getters
int					Client::getFd()				const { return (this->_fdClient); }
std::string&		Client::getBuffer()				  { return (this->_resultBuffer); }
const std::string&	Client::getBuffer()			const { return (this->_resultBuffer); }
std::string			Client::getNickname()		const { return (this->_nickname); }
std::string			Client::getUsername() 		const { return (this->_username); }
std::string 		Client::getHostname(void)   const { return (this->_hostname); }
std::string 		Client::getIpAdd(void)		const { return (this->_ipClient); }
std::string			Client::getCurrentChannel()	const { return (this->_currentChannel); }
std::map<std::string, char>* Client::GetChannel()	  { return (&this->_channels); }
bool				Client::hasPass()			const { return (this->_hasPass); }
bool				Client::hasNick()			const { return (this->_hasNick); }
bool				Client::hasUser()			const { return (this->_hasUser); }
bool				Client::isRegistered()		const { return (this->_registered); }
Server*				Client::getServer()			const { return (this->_server); }
std::string			Client::getPassword()		const { return (this->_password); }

// Setters
void	Client::setFd(int fd)								{ this->_fdClient = fd; }
void	Client::setIpAdd(std::string ipAddr)				{ this->_ipClient = ipAddr; }
void 	Client::setChannel(std::map<std::string, char> *newchannels) { this->_channels = *newchannels; }
void	Client::setNickname(std::string newname)			{ this->_nickname = newname; }
void	Client::setUsername(std::string newname)			{ this->_username = newname; }
void	Client::setHostname(std::string newname)			{ this->_hostname = newname; }
void	Client::setCurrentChannel(std::string newchannel)	{ this->_currentChannel = newchannel; }
void	Client::setHasPass(bool has)						{ this->_hasPass = has; }
void	Client::setHasNick(bool has)						{ this->_hasNick = has; }
void	Client::setHasUser(bool has)						{ this->_hasUser = has; }
void	Client::setRegistered(bool is)						{ this->_registered = is; }
void	Client::setServer(Server *server)					{ this->_server = server; }
void	Client::setPassword(std::string password)			{ this->_password = password; }

// Public member functions/ methods

//Accepting New Client
//Accepts client in the server if they dont already exist 

//Setters/Getters and an insert function
void Client::addNotification(std::string msg, char type)
{
	_notifications.insert(std::make_pair(type, msg));
}

void Client::AddChannel(std::string channelname, char type)
{
	_channels.insert(std::make_pair(channelname, type));	
}

void Client::RemoveChannel(std::string channelname)
{
	std::map<std::string, char>::iterator it = _channels.find(channelname);
	if (it != _channels.end())
		_channels.erase(it);
}
