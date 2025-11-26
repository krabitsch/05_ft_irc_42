/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvass <pvass@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:58:37 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/26 12:47:42 by pvass            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_hpp
# define SERVER_hpp

# ifdef FT_IRC_DEBUG
  #  define DBG(someCode) do { someCode; } while (0)
# else
  #  define DBG(someCode) do { } while (0)
# endif

# include <iostream>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstring>
# include <fcntl.h>
# include <poll.h>
# include <csignal>
# include <cerrno>
#include <string>
#include <map>
#include <vector>
#include "Channel.hpp"
#include "Parser.hpp"

# define RED    "\e[1;31m"
# define WHITE  "\e[0;37m"
# define GREEN  "\e[1;32m"
# define YELLOW "\e[1;33m"

class Server
{
	private:
	int							_port;
	int							_password;
	int							_fdServer;
	std::vector<Client>			_clients; 	// vector of clients
	std::vector<struct pollfd>	_fds; 		// vector of pollfd
  	std::vector<Channel>		_channels; 	//A vector of all the channels 
	
	Server();
	
	// Variables/methods global to the class:
	static bool					signalBool;

	public:
	
	// Constructors/Destructors/Operators Overlords
	Server(int port, int password);
	Server(const Server& other);
	~Server();
	Server& operator=(const Server& other);

	// Public member functions/ methods

  	// Getters

	// Setters

	//Server Functions
	void serverInit(); 				//-> server initialization
	void createSocketBindListen();	//-> server socket creation
	void acceptClient(); 			//-> accept new client
	void receiveData(int fd);		//-> receive new data from a registered client

	void closeFds(); 			//-> close file descriptors
	void clearClients(int fd);	//-> clear clients

	// Variables/methods global to the class
	static void signalHandler(int signalReceived);
	//static void SignalHandler(int signum); //-> signal handler

    //Finder Functions
    Channel* findChannel(const std::string &name);
    Client* findClient(const int fd, std::string username);

    //Commands that need the use of the server
    void nickComand(int fd, std::string newname); //Sets the new nickanem maybe change but we will see
    void join(int fd, std::string channelname); //Creates or joins a channel that exists
    void part(int fd);
    void privateMsg(std::string username, std::string msg);
	void topic(std::string channelname, int clientfd);



	void broadcastMessage(int from_fd, const std::string& msg);

	void handleMessage(int fd, const IrcCommand &cmd);
};



#endif