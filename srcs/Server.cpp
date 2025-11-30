/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:58:30 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/30 13:57:23 by krabitsc         ###   ########.fr       */
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
Server::Server(): 								_serverAdd(this), _port(-1),   _password(""),	    _fdServer(-1) {}
Server::Server(int port, std::string password): _serverAdd(this), _port(port), _password(password), _fdServer(-1) 
{
	// KR: I really don't understand why we need this: instances of Server class have access via this pointer anyway, outside the class no access to that?!
	//server.setServerAdd(&server); //Sets the server class inside to itself 
}

Server::Server(Server const& other): _serverAdd(other._serverAdd), 
									 _port(other._port), 
									 _password(other._password),
									 _fdServer(other._fdServer) {}

// Destructor:
Server::~Server() {}

// Operator overloads
Server&	Server::operator=(Server const& other)
{
	if (this != &other)
	{
		this->_serverAdd = other._serverAdd;
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
				std::cout << RED << "Client <" << this->_fds[i].fd << "> poll error/hangup" << WHITE << std::endl;
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
	this->_clients.push_back(client);												// adds client to the vector of clients
	this->_fds.push_back(NewPoll);													// adds client socket to the pollfd

	DBG({std::cout << GREEN << "Client <" << incomingClientFd << "> Connected" << WHITE << std::endl;
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
    	std::cerr << "Client " << fd << " sent too long message (>512 bytes), closing connection" << std::endl;
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
	DBG({std::cout << "Handling command: " << c << std::endl; });
	// command should already be uppercased by your parser
	if (c == "NICK")
	{
		DBG({std::cout << "Handling NICK command" << std::endl; });
		if (!cmd.parameters.empty())
		{
			/*Client *client = findClient(fd, "");
			if (!client->getNickname().empty())
				std::cout << client->getNickname() << std::endl;*/
			nickComand(fd, cmd.parameters[0]);
			//client = findClient(fd, "");
			//std::cout << client->getNickname() << std::endl;
		}
		return;
	}
	if (c == "JOIN")
	{
		std::cout << "Get here" << std::endl;
		if (!cmd.parameters.empty())
		{
			join(fd, cmd.parameters[0]);
			std::cout << _channels[0].getname() << std::endl;
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
		std::cout << "Handling PRIVMSG command" << std::endl;
		if (!cmd.parameters.empty())
		{
			std::string target = cmd.parameters[0];
			std::string msg = (cmd.parameters.size() > 1) ? cmd.parameters[1] : std::string();
			// forward to your private message handler (username=target)
			privateMsg(target, msg);
		}
		return;
	}
	if (c == "TOPIC")
	{
		if (!cmd.parameters.empty())
			topic(cmd.parameters[0], fd);
		return;
	}

	// unknown/other commands: optionally handle or reply with error
	// std::cerr << "Unhandled command: " << c << std::endl;
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


//Setters && Getters
Server *Server::getServerAdd(void) const { return (this->_serverAdd); }

void Server::setServerAdd(Server *server) { this->_serverAdd = server; }


//Finder Functions 

Channel* Server::findChannel(const std::string &name)
{
  size_t i = 0;
  while (i < _channels.size())
  {
	  if (_channels[i].getname() == name)
		return (&_channels[i]);
	i++;
  }
  return NULL;

}


Client* Server::findClient(const int fd, std::string username)
{
	int i = 0;
	while (i <_clients.size())
	{
		if (_clients[i].getUsername() == username || _clients[i].getNickname() == username)
			return (&_clients[i]);
		else if (fd > 1 && _clients[i].getFd() == fd)
			return (&_clients[i]);
	  i++;
	}
  	return (NULL);
}
