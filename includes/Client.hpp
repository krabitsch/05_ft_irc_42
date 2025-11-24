/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:57:10 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/24 16:00:02 by aruckenb         ###   ########.fr       */
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
  
	  //int fd;
    std::string nickname; //Nickname
    std::string hostname; //resolved hsotname or the IP of the user
    std::string username; //Default username set by the system for explain aruckenb or pvass //The username is bascially the home user
    std::string password; //User password

    std::map<std::string, char> channels; //Which channels the user is apart of and the char is either a m or o for member or operator
    std::string current_channel; //Should this exist or not is there a better way
    std::map<char, std::string> notifications; //Map containing invite to channel or private msg, as well as a char to indicaite what type of notification it is
	
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
    std::string GetNickname(void);
    void SetNickname(std::string newname);
    std::string GetUsername(void);

    //Client Channel Features
    std::map<std::string, char> *GetChannel(void);
    void SetChannel(std::map<std::string, char> *newchannels);
    std::string GetCurrentChannel(void);
    void SetCurrentChannel(std::string newchannel);
    void AddChannel(std::string channelname);

    //Notifications *Still unsure about this part honestly
    void addNofitication(std::string msg, char type);
	
	
};

#endif
