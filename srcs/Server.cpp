/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:58:30 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/25 12:57:56 by aruckenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// Variables/methods global to the class:
bool Server::signalBool = false;

void Server::signalHandler(int signalReceived)
{
	if (signalReceived == SIGINT || signalReceived == SIGQUIT)
	{
		std::cout << std::endl << "Signal Received!" << std::endl;
		Server::signalBool = true; // sets static boolean to true to stop the server
	}
	//(void)signum;
}

// Constructors:
Server::Server(): _port(-1), _password(-1) {}
Server::Server(int port, int password): _port(port), _password(password) {}

Server::Server(Server const& other): _port(other._port), _password(other._password) {}

// Destructor:
Server::~Server() {}

// Operator overloads
Server&	Server::operator=(Server const& other)
{
	if (this != &other)
	{
		this->_port	 	= other._port;
		this->_password = other._password;
	}
	return (*this);
}


// Public member functions/ methods
void	Server::serverInit() //-> server initialization
{

	createSocketBindListen();

	std::cout << GREEN << "Server (fd = " << _fdServer << ") Connected" << WHITE << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	while (Server::signalBool == false) // runs server until the signal is received
	{
		if((poll(&_fds[0],_fds.size(),-1) == -1) && Server::signalBool == false) // waits for an event
			throw(std::runtime_error("poll() failed"));

		for (size_t i = 0; i < _fds.size(); i++) // go through all socket file descriptors
		{
			if (_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				std::cout << RED << "Client <" << _fds[i].fd << "> poll error/hangup" << WHITE << std::endl;
				clearClients(_fds[i].fd);
				close(_fds[i].fd);
				continue ;
			}

			if (_fds[i].revents & POLLIN) 		 // checks if there is data to read
			{
				if (_fds[i].fd == _fdServer)
					acceptClient();				 // accepts a new client
				else
					receiveData(_fds[i].fd);	 // receives new data from a registered client
			}
		}
	}
	closeFds();
}


void Server::createSocketBindListen()  // create server socket creation, bind port/IP, listen
{
	// Create server socket
	_fdServer = socket(AF_INET, SOCK_STREAM, 0); // 0 picks default protocol, here IPPROTO_TCP
	if (_fdServer == -1)
		throw(std::runtime_error("Can't create socket for Server!"));

	// Set socket options as needed: socket option SO_REUSEADDR to reuse the address, socket option O_NONBLOCK for non-blocking socket
	int en = 1;
	if(setsockopt(_fdServer, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) 
		throw(std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	if (fcntl(_fdServer, F_SETFL, O_NONBLOCK) == -1)
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
	if (bind(_fdServer, (struct sockaddr*)&addrServer, sizeof(addrServer)) == -1)
		throw(std::runtime_error("Can't bind to IP/port"));

	// Mark the socket for listening
	if (listen(_fdServer, SOMAXCONN) == -1) // SOMAXCONN: how many connections it can listen to
		throw(std::runtime_error("Can't listen"));

	// add server related info for poll() function to vector of pollfd, _fds:
	NewPoll.fd	  	= _fdServer; // adds server socket to the pollfd
	NewPoll.events  = POLLIN;	// sets event to POLLIN for reading data
	NewPoll.revents = 0;		// sets revents to 0
	_fds.push_back(NewPoll);	// adds server socket to the pollfd

}

void Server::acceptClient()	// accepts new client
{
	Client 				client;
	struct sockaddr_in	addrClient;
	socklen_t			sizeClient = sizeof(addrClient);
	struct pollfd		NewPoll;
	

	int incomingClientFd = accept(_fdServer, (sockaddr *)&(addrClient), &sizeClient); // accepts new client
	if (incomingClientFd == -1)
	{
		std::cerr << "Problem with client connecting!" << std::endl;
		return ;
	}

	// sets socket option (O_NONBLOCK) for non-blocking socket
	if (fcntl(incomingClientFd, F_SETFL, O_NONBLOCK) == -1) 
	{
		std::cout << "fcntl() failed" << std::endl;
		return ;
		// returning from here is probably not good enough, since the server accepted this client and it could hang now!
	}

	// fill in client information
	NewPoll.fd	  	= incomingClientFd;	// adds client socket to the pollfd
	NewPoll.events  = POLLIN;		// sets event to POLLIN for reading data
	NewPoll.revents = 0;			// sets the revents to 0

	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];
	memset(host, 0, NI_MAXHOST);
	memset(serv, 0, NI_MAXSERV);
	client.setFd(incomingClientFd);														// sets client file descriptor
	client.setIpAdd(inet_ntop(AF_INET, &(addrClient.sin_addr), host, sizeof(host)));	// converts ip address to string and sets it
	//client.setIpAdd(inet_ntoa((addrClient.sin_addr)));								// converts ip address to string and sets it
	_clients.push_back(client);															// adds client to the vector of clients
	_fds.push_back(NewPoll);															// adds client socket to the pollfd

	//std::cout << GREEN << "Client <" << incomingClientFd << "> Connected" << WHITE << std::endl;
	int result = getnameinfo((sockaddr*)&addrClient, sizeof(addrClient), host, NI_MAXHOST, serv, NI_MAXSERV, 0);
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
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0); //-> receive the data

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
	// print received data (if bytes > 0 : we have data)
	{
		buff[bytes] = '\0';
		std::cout << YELLOW << "Client (fd = " << fd << ") Data: " << WHITE << buff;
		
		// code to process the received data: parse, check, authenticate, handle the command, etc...


		const char* replyMsg = "Received client msg...\n";
		ssize_t sent = send(fd, replyMsg, std::strlen(replyMsg), 0);
		if (sent == -1)
			std::cerr << "send() error on fd " << fd << ": " << std::strerror(errno) << std::endl;
	}

}


void	Server::clearClients(int fd)
{
	for(size_t i = 0; i < _fds.size(); i++) // removes client from the pollfd
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			break ;
		}
	}
	for(size_t i = 0; i < _clients.size(); i++) // removes client from the vector of clients
	{
		if (_clients[i].getFd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			break ;
		}
	}

}

void	Server::closeFds()
{
	for(size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << RED << "Client (fd = " << _clients[i].getFd() << ") Disconnected" << WHITE << std::endl;
		close(_clients[i].getFd());
	}
	if (_fdServer != -1)
	{
		std::cout << RED << "Server (fd = " << _fdServer << ") Disconnected" << WHITE << std::endl;
		close(_fdServer);
	}
}


//Setters && Getters
Server *Server::getServerAdd(void) const
{
	return (_serverAdd);
}

void Server::setServerAdd(Server *server)
{
	_serverAdd = server;
}


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
