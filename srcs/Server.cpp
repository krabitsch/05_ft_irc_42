/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:58:30 by krabitsc          #+#    #+#             */
/*   Updated: 2025/12/28 17:28:14 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"


// Variables/methods global to the class:
bool Server::signalBool = false;

void Server::signalHandler(int signalReceived)
{
	if (signalReceived == SIGINT || signalReceived == SIGQUIT)
	{
		std::cout << std::endl << "Signal Received!" << std::endl;
		Server::signalBool = true; // sets static boolean to true to stop the server
	}
	//(void)signalReceived;
}

// Constructors:
Server::Server(): _port(-1),
				  _password(""),
				  _fdServer(-1),
				  _serverName("") {}
				  
Server::Server(int port, std::string password): _port(port),
												_password(password),
												_fdServer(-1),
												_serverName("ircAlPeKa@42") {}

Server::Server(Server const& other): _port(other._port), 
									 _password(other._password),
									 _fdServer(other._fdServer),
									 _serverName(other._serverName) {}

// Destructor:
Server::~Server() {}

// Operator overloads
Server&	Server::operator=(Server const& other)
{
	if (this != &other)
	{
		this->_port		   = other._port;
		this->_password	   = other._password;
		this->_fdServer	   = other._fdServer;
		this->_serverName  = other._serverName;
	}
	return (*this);
}


// Public member functions/ methods

void	Server::serverInit()
{
	createSocketBindListen(); // creates server socket, binds port/IP, listens

	std::cout << GREEN << "Server (fd = " << this->_fdServer << ") Connected" << WHITE << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	while (Server::signalBool == false) // runs server until the signal is received
	{
		if((poll(&this->_fds[0],this->_fds.size(),-1) == -1) && Server::signalBool == false) // waits for an event
			throw(std::runtime_error("poll() failed"));

		for (size_t i = 0; i < this->_fds.size(); i++)	// go through all socket file descriptors
		{
			if (this->_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				std::cout << RED << "Client (fd = " << this->_fds[i].fd << ") poll error/hangup" << WHITE << std::endl;
				clearClients(this->_fds[i].fd);
				close(this->_fds[i].fd);
				continue ;
			}

			if (this->_fds[i].revents & POLLIN) 		// checks if there is data to read
			{
				if (this->_fds[i].fd == _fdServer)
					acceptClient();				 		// accepts a new client
				else
					receiveData(this->_fds[i].fd);		// receives new data from a registered client
			}
		}
	}
	closeFds();
}


void Server::createSocketBindListen()
{
	// Create server socket
	this->_fdServer = socket(AF_INET, SOCK_STREAM, 0); // 0 picks default protocol, here IPPROTO_TCP
	if (this->_fdServer == -1)
		throw(std::runtime_error("Can't create socket for Server!"));

	// Set socket options as needed: socket option SO_REUSEADDR to reuse the address, socket option O_NONBLOCK for non-blocking socket
	int enable = 1;
	if(setsockopt(this->_fdServer, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) 
		throw(std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	if (fcntl(this->_fdServer, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("failed to set option (O_NONBLOCK) on socket"));

	// fill in server information
	struct sockaddr_in		addrServer;
	struct pollfd			newPoll;
	addrServer.sin_family		= AF_INET;		 		// sets address family to ipv4
	addrServer.sin_port			= htons(this->_port);	// converts the port (host shorts) to network byte order (network shorts, in big endian)
	addrServer.sin_addr.s_addr	= INADDR_ANY;			// sets address to any local machine address
														// alternatively, use inet_pton command (for it to take any IP address: 0.0.0.0)
														// inet_pton(AF_INET, "0.0.0.0", &addrServer.sin_addr); 
	// Bind the socket to an IP/export
	if (bind(this->_fdServer, (struct sockaddr*)&addrServer, sizeof(addrServer)) == -1)
		throw(std::runtime_error("Can't bind to IP/port"));

	// Mark the socket for listening
	if (listen(this->_fdServer, SOMAXCONN) == -1) // SOMAXCONN: how many connections it can listen to
		throw(std::runtime_error("Can't listen"));

	// add server related info for poll() function to vector of pollfd, _fds:
	newPoll.fd	  	= this->_fdServer;	// adds server socket to the pollfd
	newPoll.events  = POLLIN;			// sets event to POLLIN for reading data
	newPoll.revents = 0;				// sets revents to 0
	this->_fds.push_back(newPoll);		// adds server socket to the pollfd

}

void Server::acceptClient()	// accepts new client
{
	struct sockaddr_in	addrClient;
	socklen_t			sizeClient = sizeof(addrClient);
	struct pollfd		newPoll;

	int incomingClientFd = accept(this->_fdServer, (sockaddr *)&(addrClient), &sizeClient); // accepts new client
	if (incomingClientFd == -1)
	{
		std::cerr << "Problem with client connecting!" << std::endl;
		return ;
	}

	// sets socket option (O_NONBLOCK) for non-blocking socket
	if (fcntl(incomingClientFd, F_SETFL, O_NONBLOCK) == -1) 
	{
		std::cout << "fcntl() failed. Client fd can't be set to non-blocking. Dropping client." << std::endl;
		close(incomingClientFd);
		return ;
	}

	// create Client instance (as pointer) and fill in client information
	Client*  client = new Client();
	newPoll.fd	  	= incomingClientFd;	// adds client socket to the pollfd
	newPoll.events  = POLLIN;			// sets event to POLLIN for reading data
	newPoll.revents = 0;				// sets the revents to 0

	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];
	memset(host, 0, NI_MAXHOST);
	memset(serv, 0, NI_MAXSERV);
	client->setFd(incomingClientFd);													// sets client file descriptor
	client->setIpAdd(inet_ntop(AF_INET, &(addrClient.sin_addr), host, sizeof(host)));// converts ip address to string and sets it
	client->setServer(this);
	this->_clients.push_back(client);												// adds client to the vector of clients
	this->_fds.push_back(newPoll);													// adds client socket to the pollfd

	sendNotice(incomingClientFd, "AUTH", "*** Looking up your hostname...");
	int result = getnameinfo((sockaddr*)&addrClient, sizeof(addrClient), host, NI_MAXHOST, serv, NI_MAXSERV, NI_NAMEREQD); // require a hostname
	if (result == 0)
	{
		client->setHostname(host);
		sendNotice(incomingClientFd, "AUTH", "*** Found your hostname: " + client->getHostname());
	}
	else
	{
		client->setHostname(client->getIpAdd()); // or host string from inet_ntop
		sendNotice(incomingClientFd, "AUTH", "*** Couldn't look up your hostname");
	}
	sendNotice(incomingClientFd, "AUTH", "*** Checking Ident"); // send fake msgs re Ident (has nothing to do with IRC authentification)
	sendNotice(incomingClientFd, "AUTH", "*** No Ident response");

	std::cout << GREEN << "Client (fd = " << incomingClientFd << ") Connected" << WHITE << std::endl;
	//int result = getnameinfo((sockaddr*)&addrClient, sizeof(addrClient), host, NI_MAXHOST, serv, NI_MAXSERV, 0);
	if (result == 0)
	{
		std::cout << host << " (Client (fd = " << incomingClientFd << "))"
				  << " connected FROM port " << ntohs(addrClient.sin_port)
				  << " TO server port " << _port
			  	<< std::endl;
	}

}


void	Server::receiveData(int fd)	// receives new data from a registered client
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0); // receives data

	if(bytes < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK) // no data available right now on a non-blocking socket; just ignore.
			return ;
		std::cerr << RED << "recv() error on fd " << fd << ": " << std::strerror(errno) << WHITE << std::endl;
		clearClients(fd);
		close(fd);
	}
	if (bytes == 0)
	{
		std::cout << RED << "Client (fd = " << fd << ") Disconnected" << WHITE << std::endl;
		clearClients(fd);
		close(fd);
		return ;
	}
	
	// bytes > 0 : we have data
	buff[bytes] = '\0';
	
	// find client associated with this fd and append (valid) bytes to client's buffer
	Client*			client 		 = findClientByFd(fd);
	std::string&	resultBuffer = client->getBuffer();
	resultBuffer.append(buff, bytes);
	static const size_t IRC_MAX_LINE = 512; // 512 includes CRLF.
	// extract complete messages (delimited by "\r\n"), parse and handle the command, etc...
	while (true)
	{
		size_t posEOL = resultBuffer.find("\r\n");
		if (posEOL == std::string::npos)
		{
			// no full line yet (no \r\n); avoid unbounded growth by dropping potentially malicious clients if msg too long ( strict RFC enforcement)
			if (resultBuffer.size() > IRC_MAX_LINE)
			{
				sendNumeric(fd, 417, "*", std::vector<std::string>(), "Input line too long"); // 417 = too long line
				clearClients(fd);
				close(fd);
			}
			break ;
		}
		// have a complete line (0 to eol)
		if (posEOL > IRC_MAX_LINE) // ... but line is too long
		{
			sendNumeric(fd, 417, "*", std::vector<std::string>(), "Input line too long");
			clearClients(fd);
			close(fd);
			return ;
		}
		// have a complete line (0 to eol) within 512 line limit
		std::string message = resultBuffer.substr(0, posEOL);
		resultBuffer.erase(0, posEOL + 2);

		IrcCommand command = parseMessage(message);
		DBG({command.print(); });
		this->handleMessage(fd, command);
		//this->broadcastMessage(fd, message + "\r\n");
	}
}

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

void Server::broadcastToChannel(const std::string& channelName, const std::string& msg, int exceptFd)
{
	Channel* channel = findChannel(channelName);
	if (!channel)
		return ;

	std::vector<Client*>* members = channel->getMembers();
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

		sendConstructedMsg(toFd, msg); // msg ends with \r\n
	}
}

void Server::handleMessage(int fd, const IrcCommand &cmd)
{
	std::string c = cmd.command;
	Client*		client = findClientByFd(fd);
	if (!client)
		return ;

	DBG({std::cout << "Handling command: " << c << std::endl; });

	// Commands PASS, NICK, USER and QUIT are accepted even before registration is completed
	if (c == "PASS")
	{
		DBG({std::cout << "Handling PASS command" << std::endl; });
		this->passCommand(*client, cmd);
		return ;
	}
	if (c == "NICK")
	{
		DBG({std::cout << "Handling NICK command" << std::endl; });
		this->nickCommand(*client, cmd);
		return ;
	}
	if (c == "USER")
	{
		DBG({std::cout << "Handling USER command" << std::endl; });
		this->userCommand(*client, cmd);
		return ;
	}
	if (c == "QUIT")
	{
		DBG({std::cout << "Handling QUIT command" << std::endl; });
		if (cmd.parameters.empty())
			quitCommand("", fd);
		else
			quitCommand(cmd.parameters[0], fd);
		return ;
	}
	
	DBG({
	std::cout << YELLOW << "[REG CHECK] Cliend (fd = " << fd << ")"
			  << " PASS=" << (client->hasPass() ? "true" : "false")
			  << " NICK=" << (client->hasNick() ? "true" : "false")
			  << " USER=" << (client->hasUser() ? "true" : "false")
			  << " REGISTERED=" << (client->isRegistered() ? "true" : "false")
			  << WHITE << std::endl; });

	
	// check if client is registered 
	if (!client->isRegistered())
	{
		// 451 ERR_NOTREGISTERED
		sendNumeric(fd, 451, "*", std::vector<std::string>(),
					"You have not registered");
		return ;
	}

	// from here on, only registered clients:
	if (c == "JOIN")
	{	
		//Topic Numeric Replies
		/*Numeric Replies:

	   	ERR_NEEDMOREPARAMS			ERR_BANNEDFROMCHAN
		ERR_INVITEONLYCHAN			ERR_BADCHANNELKEY
		ERR_CHANNELISFULL			ERR_BADCHANMASK
		ERR_NOSUCHCHANNEL			ERR_TOOMANYCHANNELS
		RPL_TOPIC*/
		
		if (!cmd.parameters.empty()) // move this logic inside the command handling
		{
			if (cmd.parameters.size() == 2)
				join(fd, cmd.parameters[0], cmd.parameters[1]);
			else
				join(fd, cmd.parameters[0], "");
			//std::cout << "User has joined channel: " << _channels[0].getname() << std::endl;
			std::cout << "User has joined channel: " << cmd.parameters[0] << std::endl;
		}
		else 
		{
			// 461 ERR_NEEDMOREPARAMS
			this->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "JOIN"),
						"Not enough parameters");
		}
		return;
	}
	if (c == "PART")
	{
		// your part() currently takes only fd; if you need channel name, change signature
		part(fd);
		return;
	}
	if (c == "PRIVMSG")
	{
		/*
		*************************
		Still need to implement:

		404	 ERR_CANNOTSENDTOCHAN
						"<channel name> :Cannot send to channel"

				- Sent to a user who is either (a) not on a channel
				  which is mode +n or (b) not a chanop (or mode +v) on
				  a channel which has mode +m set and is trying to send
				  a PRIVMSG message to that channel.
		
		413	 ERR_NOTOPLEVEL
						"<mask> :No toplevel domain specified"
		
		414	 ERR_WILDTOPLEVEL
						"<mask> :Wildcard in toplevel domain"

				- 412 - 414 are returned by PRIVMSG to indicate that
				  the message wasn't delivered for some reason.
				  ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that
				  are returned when an invalid use of
				  "PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.
		
		407	 ERR_TOOMANYTARGETS
						"<target> :Duplicate recipients. No message \

		*************************
		*/


		std::cout << "Handling PRIVMSG command" << std::endl;
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
	if (c == "TOPIC")
	{
		//Topic Numeric Replies
		/*Numeric Replies:

	   	ERR_NEEDMOREPARAMS			ERR_NOTONCHANNEL
		RPL_NOTOPIC					RPL_TOPIC
		ERR_CHANOPRIVSNEEDEDL*/
		//Extra: Look into if topic is done correctly
		
		if (!cmd.parameters.empty())
		{
			topic(cmd.parameters[0], fd);
		}
		else
		 	topic("", fd);
		return;
	}
	if (c == "KICK")
	{
		//Kick Numeric Replies //AL: Added all of them other then ERR_BADCHANMASK, unsure what that really is
		/*Numeric Replies:

		ERR_NEEDMOREPARAMS			ERR_NOSUCHCHANNEL
		ERR_BADCHANMASK				ERR_CHANOPRIVSNEEDED
		ERR_NOTONCHANNEL*/
		
		if (!cmd.parameters.empty())
		{
			if (cmd.parameters.size() < 2)
			{
				//ERR_NEEDMOREPARAMS 461
				this->sendNumeric(fd, 461, "", std::vector<std::string>(), "Not enough parameters");
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
				this->sendNumeric(fd, 403, "", std::vector<std::string>(), "No such channel");
			}
			return;
		}
		else 
		{
			//ERR_NEEDMOREPARAMS 461
			this->sendNumeric(fd, 461, "", std::vector<std::string>(), "Not enough parameters");
			return ;
		}
	}
	if (c == "MODE")
	{
		//Mode Numeric Replies
		/*Numeric Replies:

	   
		ERR_NEEDMOREPARAMS				RPL_CHANNELMODEIS
		ERR_CHANOPRIVSNEEDED			ERR_NOSUCHNICK //Dont think we need this one
		ERR_NOTONCHANNEL				ERR_KEYSET
		RPL_BANLIST						RPL_ENDOFBANLIST //I dont think we need  since we dont have a ban list 
		ERR_UNKNOWNMODE					ERR_NOSUCHCHANNEL

		ERR_USERSDONTMATCH				RPL_UMODEIS
		ERR_UMODEUNKNOWNFLAG*/
		
		//sets modes for channels and users, modes need to be tested
		if (!cmd.parameters.empty())
		{
			Channel* channel = findChannel(cmd.parameters[0]);
			if (channel == NULL)
			{
				//ERR_NOSUCHCHANNEL 403
				this->sendNumeric(fd, 403, "", std::vector<std::string>(), "No such channel");
			}
			else
			{
				if (cmd.parameters.size() == 2)
					channel->mode(fd, cmd.parameters[1], "");
				else if (cmd.parameters.size() == 3)
					channel->mode(fd, cmd.parameters[1], cmd.parameters[2]);
			}
			return ;
		}
		else 
		{
			//ERR_NEEDMOREPARAMS 461
			this->sendNumeric(fd, 461, "", std::vector<std::string>(), "Not enough parameters");
			return ;
		}
	}
	if (c == "INVITE") //Needs to be tested
	{
		//Invite Numeric Replies
		/*Numeric Replies:

	   
		ERR_NEEDMOREPARAMS				ERR_NOSUCHNICK
		ERR_NOTONCHANNEL				ERR_USERONCHANNEL
		ERR_CHANOPRIVSNEEDED
		RPL_INVITING					RPL_AWAY*/
		
		if (cmd.parameters.empty())
		{
			//ERR_NEEDMOREPARAMS
			this->sendNumeric(fd, 461, "", std::vector<std::string>(), "Not enough parameters");
			return ;
		}
		Channel* channel = findChannel(cmd.parameters[0]); //Check if they create a new channel if one doesnt exist
		if (channel == NULL)
		{
			//ERR_NOSUCHCHANNEL 403
			this->sendNumeric(fd, 403, "", std::vector<std::string>(), "No such channel");
			return ;
		}
		channel->invite(cmd.parameters[1], fd);
		return ;
	}
	
	//Special Debugging Commands these are custom commands so they dont follow the IRC protocol
	if (c == "OP") //Al: The error handling hasnt been implemented for this function 
	{	
		if (cmd.parameters.empty())
			return ;
		Channel* channel = findChannel(findClientByFd(fd)->getCurrentChannel());
		if (channel == NULL)
		{
			this->sendNumeric(fd, 403, "", std::vector<std::string>(), "No such channel");
			return ;
		}
		if (cmd.parameters[0] == "-u")
		{
			channel->UnsetOperator(cmd.parameters[1], fd);
		}
		else if (cmd.parameters[0] == "-u")
		{
			channel->SetOperator(cmd.parameters[1], fd);
		}
		return ;
	}
	if (c == "LIST")
	{
		if (cmd.parameters.empty())
		{	//prints out all user channels
			Client *client = findClientByFd(fd);
			std::map<std::string, char> *userchannels = client->GetChannel();
			if (userchannels != NULL)
			{
				std::cout << client->getNickname() << " this users channels!" << std::endl;
				for (std::map<std::string, char>::iterator it = userchannels->begin(); it != userchannels->end(); ++it)
				{
					std::string channelName = it->first;   // key (channel name)
					char channelType = it->second;		 // value (member 'm' or operator 'o')
					
					std::cout << "Channel: " << channelName << " Type: " << channelType << std::endl;
				}
			}
		}
		else if (cmd.parameters[0] == "server")
		{
			std::cout << "Amount of channels on server: " << this->_channels.size() << std::endl;
			std::cout << "Amount of clients on server: " << this->_clients.size() << std::endl;
		}
		else
		{	//prints out all members of a channel
			Channel* channel = findChannel(cmd.parameters[0]);
			std::cout << channel->getMembersize() << std::endl;
			channel->printMembers();
		}
		return ;
	}

	// unknown/other commands: handle by sending 421 numeric (unknown commands)
	this->sendNumeric(fd, 421, client->getNickname(), std::vector<std::string>(1, c), "Unknown command");
}



void	Server::clearClients(int fd)
{
	for(size_t i = 0; i < this->_fds.size(); i++) // removes client from the pollfd
	{
		// remove client fd from poll list
		if (this->_fds[i].fd == fd)
		{
			this->_fds.erase(this->_fds.begin() + i);
			break ;
		}
	}

	// find client-to-be-removed pointer in _clients, remove from all channels first, then remove
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		Client* toBeRemoved = this->_clients[i];
		if (toBeRemoved && toBeRemoved->getFd() == fd)
		{
			const std::string nickTBR = toBeRemoved->getNickname();
			const std::string userTBR = toBeRemoved->getUsername();

			for (size_t c = 0; c < this->_channels.size(); c++)
			{
				if (this->_channels[c])
				{
					if (!nickTBR.empty())
						this->_channels[c]->RemoveMember(nickTBR);
					if (!userTBR.empty())
						this->_channels[c]->RemoveMember(userTBR);
				}
			}
			delete toBeRemoved;
			this->_clients.erase(this->_clients.begin() + i);
			break ;
		}
	}
}

void	Server::closeFds()
{
	// close + delete clients
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (this->_clients[i])
		{
			std::cout << RED << "Client (fd = " << this->_clients[i]->getFd() << ") Disconnected" << WHITE << std::endl;
			close(this->_clients[i]->getFd());
			delete (this->_clients[i]);
		}
	}
	this->_clients.clear();

	// delete channels (Server owns them)
	for (size_t i = 0; i < this->_channels.size(); i++)
		delete (this->_channels[i]);
	this->_channels.clear();

	// close server socket
	if (this->_fdServer != -1)
	{
		std::cout << RED << "Server (fd = " << this->_fdServer << ") Disconnected" << WHITE << std::endl;
		close(this->_fdServer);
	}
}

//Finder Functions 

Channel*	Server::findChannel(const std::string &name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (this->_channels[i] && this->_channels[i]->getname() == name)
			return (this->_channels[i]);
	}
	return (NULL);
}


Client*		Server::findClientByNickOrUser(const int fd, std::string username)
{
	size_t i = 0;
	while (i < this->_clients.size())
	{
		Client* cl = this->_clients[i];
		if (!cl)
		{
			i++; 
			continue ;
		}
		if (cl->getUsername() == username || cl->getNickname() == username)
			return (cl);
		else if (fd > 1 && cl->getFd() == fd)
			return cl;
		i++;
	}
  	return (NULL);
}

Client*		Server::findClientByFd(const int fd) 
{
	size_t i = 0;
	while (i < this->_clients.size())
	{
		if (this->_clients[i] && this->_clients[i]->getFd() == fd)
			return (this->_clients[i]);
	  i++;
	}
  	return (NULL);
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

	// 001 RPL_WELCOME
	this->sendNumeric(client.getFd(), 1, nick,std::vector<std::string>(),
				"Welcome to the ft_irc server " + nick);

	// 002 RPL_YOURHOST
	this->sendNumeric(client.getFd(), 2, nick, std::vector<std::string>(),
				"Your host is " + this->_serverName + ", running version 1.0");

	// 003 RPL_CREATED
	this->sendNumeric(client.getFd(), 3, nick, std::vector<std::string>(),
				"This server was created 2025-11-30");


}


void Server::tryRegisterClient(Client &client)
{
	if (client.isRegistered())
		return ;
	
	if (!client.hasPass())
		return ;
	if (!client.hasNick())
		return ;
	if (!client.hasUser())
		return ;

	// client not yet registered and all conditions met -> set as registered and send welcome
	client.setRegistered(true);
	this->sendWelcome(client);
}
