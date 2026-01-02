/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:58:37 by krabitsc          #+#    #+#             */
/*   Updated: 2025/12/28 19:51:02 by krabitsc         ###   ########.fr       */
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
# include <string>
# include <vector>
# include "Channel.hpp"
# include "Parser.hpp"
# include "Replies.hpp"
# include <sstream>
# include <cctype>

# define RED	"\e[1;31m"
# define WHITE  "\e[0;37m"
# define GREEN  "\e[1;32m"
# define YELLOW "\e[1;33m"

class Server
{
	private:
	
	
	int							_port;
	std::string					_password;
	int							_fdServer;
	std::vector<Client*>		_clients; 	// vector of clients
	std::vector<struct pollfd>	_fds; 		// vector of pollfd
  	std::vector<Channel*>		_channels; 	// A vector of all the channels 
	
	std::string					_serverName; 
	
	Server();
	
	// Variables/methods global to the class:
	static bool					signalBool;

	// Server Functions (internal)
	void		createSocketBindListen();	// server socket creation
	void		acceptClient(); 			// accept new client
	void		receiveData(int fd);		// receive new data from a registered client
	void		clearClients(int fd);		// clear clients

	// Commands that need the use of the server
	void		passCommand(Client &client, const IrcCommand &cmd);
	void		nickCommand(Client &client, const IrcCommand &cmd);
	void		userCommand(Client &client, const IrcCommand &cmd);
	void		quitCommand(std::string message, int fd);
	void		join(int fd, std::string channelname, std::string pass); //Creates or joins a channel that exists
	void		part(int fd);
	void		privateMsg(int senderFd, std::string target, std::string msg);
	std::string makePrivmsg(const std::string &prefix, const std::string &target, const std::string &msg);
	void		topic(std::string channelname, int clientfd);

	void		handleMessage(int fd, const IrcCommand &cmd);
	void		broadcastMessage(int from_fd, const std::string& msg); // probably don't need this general broadcastMessage (currently unused)
	void		broadcastToChannel(const std::string& channelName, const std::string& msg, int exceptFd);

	// NICK helpers:
	void		broadcastNickChange(Client& client, const std::string& oldNick, const std::string& newNick);


	void		sendWelcome(Client &client);
	void		tryRegisterClient(Client &client);

	public:
	
	// Constructors/Destructors/Operators Overlords
	Server(int port, std::string password);
	Server(const Server& other);
	~Server();
	Server& operator=(const Server& other);

	// Public member functions/ methods

	// constructing the messages the server sends in the right format AL: Made these public since i dont see a difference
	void		sendConstructedMsg(int fd, const std::string &line);
	void		sendMessage(int fd, const std::string &prefix, const std::string &command,
					const std::vector<std::string> &params, const std::string &trailing);
	void		sendNumeric(int fd, int code, const std::string &target,
					const std::vector<std::string> &params, const std::string &trailing);
	void		sendNotice(int fd, const std::string &target, const std::string &text);

  	// Getters

	// Setters

	//Server Functions
	void		serverInit(); 				// server initialization
	void		closeFds(); 				// close file descriptors

	// Variables/methods global to the class
	static void	signalHandler(int signalReceived);

	// Finder Functions
	Channel*	findChannel(const std::string &name);
	Client*		findClientByNickOrUser(const int fd, std::string username);
	Client*		findClientByFd(const int fd);

};



#endif