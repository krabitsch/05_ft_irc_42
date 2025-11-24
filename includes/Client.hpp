/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:57:10 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/24 14:59:31 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_hpp
# define CLIENT_hpp

# ifdef FT_IRC_DEBUG
  #  define DBG(someCode) do { someCode; } while (0)
# else
  #  define DBG(someCode) do { } while (0)
# endif

# include <string>
# include <vector>
# include <map>

class Client
{
	private:
		int			_fdClient;
		std::string	_ipClient;
	
		/*
		int fd; //Fd Socket
		std::string nickname; //Nickname
		std::string hostname; //resolved hsotname or the IP of the user
		std::string username; //Default username set by the system for explain aruckenb or pvass //The username is bascially the home user
		std::string password; //User password

		std::map<std::string, char> channels; //Which channels the user is apart of and the char is either a m or o for member or operator
		std::string current_channel; //Should this exist or not is there a better way
		std::vector<std::string> notifications; //This array contains all notifications, invites to channels and private messages
		*/
	
	public:
		// Constructors:
		Client();
		Client(const Client& other);

		// Destructor:
		~Client();

		// Operator overloads
		Client &operator=(const Client& other);

		// Public member functions/ methods

		// Getters
		int 						getFd() const;
		/*
		//Client Info
		std::string 				GetNickname(void);
		int							GetFdSocket(void);
		std::string 				GetUsername(void);
		//Client Channel Features
		std::map<std::string, char> *GetChannel(void);
		*/

		// Setters
		void						setFd(int fd);
		void						setIpAdd(std::string ipadd);

		/*
		//Client Info
		void						SetNickname(std::string newname);
		//Client Channel Features
		void 						SetChannel(std::map<std::string, char> *newchannels);
		*/

		//Notifications *Still unsure about this part honestly

		// Variables/methods global to the class

		// Exception classes
	
	
};

#endif