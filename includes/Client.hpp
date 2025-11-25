/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:57:10 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/25 15:58:14 by aruckenb         ###   ########.fr       */
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
# include <map>

class Server;

class Client
{
	private:
	Server 		*_server;
	int			_fdClient;
	std::string	_ipClient;

    std::string _nickname; //Nickname
    std::string _hostname; //resolved hsotname or the IP of the user
    std::string _username; //Default username set by the system for explain aruckenb or pvass //The username is bascially the home user
    std::string _password; //User password

    std::map<std::string, char> _channels; //Which channels the user is apart of and the char is either a m or o for member or operator
    std::string _currentChannel; //Should this exist or not is there a better way
    std::map<char, std::string> _notifications; //Map containing invite to channel or private msg, as well as a char to indicaite what type of notification it is
	
	public:
	
	// Constructors/Destructors/Operators Overlords
	Client();
	Client(const Client& other);
	~Client();
	Client &operator=(const Client& other);

	// Public member functions/ methods

	// Getters
	int 						getFd() const;
	std::string 				getNickname(void) const;
	std::string 				getUsername(void) const;
	std::string 				getCurrentChannel(void) const;
	std::map<std::string, char> *GetChannel(void); //Not const due to returning a pointer

	// Setters
	void						setFd(int fd);
	void						setIpAdd(std::string ipadd);
	void						setUsername(std::string newname);
	void 						setNickname(std::string newname);
	void 						setChannel(std::map<std::string, char> *newchannels);
    void 						setCurrentChannel(std::string newchannel);

	//Notifications *Still unsure about this part honestly

	// Variables/methods global to the class

    //Client Channel Features
    void AddChannel(std::string channelname);

    //Notifications *Still unsure about this part honestly
    void addNofitication(std::string msg, char type);
	
	
};

#endif
