/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:58:30 by krabitsc          #+#    #+#             */
/*   Updated: 2025/12/04 11:00:50 by aruckenb         ###   ########.fr       */
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
												_serverName("ircAlPeKa@42")
{
	// KR: I really don't understand why we need this: instances of Server class have access via this pointer anyway, outside the class no access to that?!
	//server.setServerAdd(&server); //Sets the server class inside to itself 
}

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
		this->_port		 = other._port;
		this->_password	 = other._password;
		this->_fdServer  = other._fdServer;
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
	int en = 1;
	if(setsockopt(this->_fdServer, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) 
		throw(std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	if (fcntl(this->_fdServer, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("failed to set option (O_NONBLOCK) on socket"));

	// fill in server information
	struct sockaddr_in		addrServer;
	struct pollfd			NewPoll;
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
	NewPoll.fd	  	= this->_fdServer;	// adds server socket to the pollfd
	NewPoll.events  = POLLIN;			// sets event to POLLIN for reading data
	NewPoll.revents = 0;				// sets revents to 0
	this->_fds.push_back(NewPoll);		// adds server socket to the pollfd

}

void Server::acceptClient()	// accepts new client
{
	Client 				client;
	struct sockaddr_in	addrClient;
	socklen_t			sizeClient = sizeof(addrClient);
	struct pollfd		NewPoll;

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

	// fill in client information
	NewPoll.fd	  	= incomingClientFd;	// adds client socket to the pollfd
	NewPoll.events  = POLLIN;			// sets event to POLLIN for reading data
	NewPoll.revents = 0;				// sets the revents to 0

	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];
	memset(host, 0, NI_MAXHOST);
	memset(serv, 0, NI_MAXSERV);
	client.setFd(incomingClientFd);													// sets client file descriptor
	client.setIpAdd(inet_ntop(AF_INET, &(addrClient.sin_addr), host, sizeof(host)));// converts ip address to string and sets it
	client.setServer(this);
	this->_clients.push_back(client);												// adds client to the vector of clients
	this->_fds.push_back(NewPoll);													// adds client socket to the pollfd

	sendNotice(incomingClientFd, "AUTH", "*** Looking up your hostname...");
	int result = getnameinfo((sockaddr*)&addrClient, sizeof(addrClient), host, NI_MAXHOST, serv, NI_MAXSERV, NI_NAMEREQD); // require a hostname
	if (result == 0)
	{
		std::string hostname(host);
		sendNotice(incomingClientFd, "AUTH", "*** Found your hostname: " + hostname);
	}
	else
		sendNotice(incomingClientFd, "AUTH", "*** Couldn't look up your hostname");
	
	sendNotice(incomingClientFd, "AUTH", "*** Checking Ident"); // send fake msgs re Ident (has nothing to do with IRC authentification)
	sendNotice(incomingClientFd, "AUTH", "*** No Ident response");

	DBG({std::cout << GREEN << "Client (fd = " << incomingClientFd << ") Connected" << WHITE << std::endl;
		int result = getnameinfo((sockaddr*)&addrClient, sizeof(addrClient), host, NI_MAXHOST, serv, NI_MAXSERV, 0);
		if (result == 0)
		{
			std::cout << host << " (Client (fd = " << incomingClientFd << "))"
					  << " connected FROM port " << ntohs(addrClient.sin_port)
					  << " TO server port " << _port
				  	<< std::endl;
		} });

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
	Client*			client 		 = findClient(fd, "");
	std::string&	resultBuffer = client->getBuffer();
	resultBuffer.append(buff, bytes);
	static const size_t IRC_MAX_MSG = 512;
	if (resultBuffer.size() > IRC_MAX_MSG) // strict RFC enforcement, drop client if msg too long
	{
		std::cerr << "Client (fd = " << fd << ") sent too long message (>512 bytes), closing connection" << std::endl;
		clearClients(fd);
		close(fd);
		return ;
	}

	//std::cout << YELLOW << "Client (fd = " << fd << ") Data: " << WHITE << result << WHITE;
			
	// extract complete messages (delimited by "\r\n"), parse and handle the command, etc...
	size_t pos;
	while ((pos = resultBuffer.find("\r\n")) != std::string::npos)
	{
   		std::string message = resultBuffer.substr(0, pos);
		resultBuffer.erase(0, pos + 2); // +2 to remove the \r\n

   		//std::cout << "Received complete message: [" << message << "]\n";

		//const char* replyMsg = "Received client msg...\n";
		//ssize_t sent = send(fd, replyMsg, std::strlen(replyMsg), 0);
		//ssize_t sent = send(fd, message.c_str(), message.size(), 0);

		IrcCommand command = parseMessage(message);
		DBG({command.print(); });
		this->broadcastMessage(fd, message + "\r\n");
		this->handleMessage(fd, command);
		//if (sent == -1)
		//	std::cerr << "send() error on fd " << fd << ": " << std::strerror(errno) << std::endl;
	}

}


void Server::broadcastMessage(int from_fd, const std::string& msg) 
{
	for (size_t i = 0; i < _clients.size(); ++i) 
	{
		int client_fd = _clients[i].getFd();
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

void Server::handleMessage(int fd, const IrcCommand &cmd)
{
	std::string c = cmd.command;
	Client*		client = findClient(fd, "");
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
		quit(fd);
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
		return;
	}

	// from here on, only registered clients:
	if (c == "JOIN")
	{
		if (!cmd.parameters.empty()) // move this logic inside the command handling
		{
			if (cmd.parameters.size() == 2)
				join(fd, cmd.parameters[0], cmd.parameters[1]);
			else
				join(fd, cmd.parameters[0], "");
			std::cout << "User has joined channel: " << _channels[0].getname() << std::endl;
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

		404     ERR_CANNOTSENDTOCHAN
                        "<channel name> :Cannot send to channel"

                - Sent to a user who is either (a) not on a channel
                  which is mode +n or (b) not a chanop (or mode +v) on
                  a channel which has mode +m set and is trying to send
                  a PRIVMSG message to that channel.
		
		413     ERR_NOTOPLEVEL
                        "<mask> :No toplevel domain specified"
		
		414     ERR_WILDTOPLEVEL
                        "<mask> :Wildcard in toplevel domain"

                - 412 - 414 are returned by PRIVMSG to indicate that
                  the message wasn't delivered for some reason.
                  ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that
                  are returned when an invalid use of
                  "PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.
		
		407     ERR_TOOMANYTARGETS
                        "<target> :Duplicate recipients. No message \

		*************************
		*/


		std::cout << "Handling PRIVMSG command" << std::endl;
		if (cmd.parameters.empty() || (cmd.parameters.size() == 1 && cmd.has_trailing == true))
		{
			//411 ERR_NORECIPIENT
			sendNumeric(fd, 411, this->findClient(fd)->getNickname(), std::vector<std::string>(),
					"No recipient given (PRIVMSG)");
		}
		else if (cmd.parameters.size() == 1)
		{
			//412 ERR_NOTEXTTOSEND
			sendNumeric(fd, 412, this->findClient(fd)->getNickname(), std::vector<std::string>(),
				":No text to send");
			return;
		}
		else
		{
			for (int i = 0; i < cmd.parameters.size() - 1; i++)
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
		if (!cmd.parameters.empty())
			topic(cmd.parameters[0], fd);
		else
		 	topic("", fd);
		return;
	}
	if (c == "KICK") //Needs to be tested so far hasnt kick the user
	{
		if (!cmd.parameters.empty())
		{
			Channel *channel = findChannel(cmd.parameters[0]);
			if (channel)
				channel->kick(cmd.parameters[1], fd);
			else
				this->sendNumeric(fd, 403, "", std::vector<std::string>(), "No such channel");
		}
		return;
	}
	if (c == "MODE")
	{
		//sets modes for channels and users, modes need to be tested
		if (!cmd.parameters.empty())
		{
			Channel* channel = findChannel(cmd.parameters[0]);
			if (channel == NULL)
				this->sendNumeric(fd, 403, "", std::vector<std::string>(), "No such channel");
			else
			{
				if (cmd.parameters.size() == 2)
					channel->mode(fd, cmd.parameters[1], "");
				else if (cmd.parameters.size() == 3)
					channel->mode(fd, cmd.parameters[1], cmd.parameters[2]);
			}
			return ;
		}
	}
	if (c == "INVITE") //Needs to be tested
	{
		if (cmd.parameters.empty())
			return ;
		Channel* channel = findChannel(findClient(fd, "")->getCurrentChannel());
		if (channel == NULL)
		{
			this->sendNumeric(fd, 403, "", std::vector<std::string>(), "No such channel");
			return ;
		}
		channel->invite(cmd.parameters[0], fd);
		return ;
	}
	
	//Special Debugging Commands
	if (c == "OP") //Al: The error handling hasnt been implemented for this function 
	{	
		if (cmd.parameters.empty())
			return ;
		Channel* channel = findChannel(findClient(fd, "")->getCurrentChannel());
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
			Client *client = findClient(fd, "");
			std::map<std::string, char> *userchannels = client->GetChannel();
			if (userchannels != NULL)
			{
				std::cout << client->getNickname() << " this users channels!" << std::endl;
				for (std::map<std::string, char>::iterator it = userchannels->begin(); it != userchannels->end(); ++it)
				{
					std::string channelName = it->first;   // key (channel name)
					char channelType = it->second;         // value (member 'm' or operator 'o')
					
					std::cout << "Channel: " << channelName << " Type: " << channelType << std::endl;
				}
			}
		}
		else
		{	//prints out all members of a channel
			Channel* channel = findChannel(cmd.parameters[0]);
			std::cout << channel->getMembersize() << std::endl;
		}
		return ;
	}

	// unknown/other commands: handle by sending 421 numeric (unknown commands)
	this->sendNumeric(fd, 421, client->getNickname(), std::vector<std::string>(1, c),
				"Unknown command");
}



void	Server::clearClients(int fd)
{
	for(size_t i = 0; i < this->_fds.size(); i++) // removes client from the pollfd
	{
		if (this->_fds[i].fd == fd)
		{
			this->_fds.erase(this->_fds.begin() + i);
			break ;
		}
	}
	for(size_t i = 0; i < this->_clients.size(); i++) // removes client from the vector of clients
	{
		if (this->_clients[i].getFd() == fd)
		{
			this->_clients.erase(this->_clients.begin() + i);
			break ;
		}
	}

}

void	Server::closeFds()
{
	for(size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << RED << "Client (fd = " << this->_clients[i].getFd() << ") Disconnected" << WHITE << std::endl;
		close(_clients[i].getFd());
	}
	if (_fdServer != -1)
	{
		std::cout << RED << "Server (fd = " << this->_fdServer << ") Disconnected" << WHITE << std::endl;
		close(_fdServer);
	}
}

//Finder Functions 

Channel* Server::findChannel(const std::string &name)
{
  size_t i = 0;
  while (i < _channels.size())
  {
	  if (this->_channels[i].getname() == name)
		return (&this->_channels[i]);
	i++;
  }
  return NULL;

}

Client* Server::findClient(const int fd, std::string username)
{
	int i = 0;
	while (i <this->_clients.size())
	{
		if (this->_clients[i].getUsername() == username || this->_clients[i].getNickname() == username)
			return (&this->_clients[i]);
		else if (fd > 1 && this->_clients[i].getFd() == fd)
			return (&this->_clients[i]);
	  i++;
	}
  	return (NULL);
}

Client* Server::findClient(const int fd) {
	int i = 0;
	while (i <this->_clients.size())
	{
		if (fd > 1 && this->_clients[i].getFd() == fd)
			return (&this->_clients[i]);
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

	// All conditions met -> mark as registered and send welcome
	client.setRegistered(true);
	this->sendWelcome(client);
}
