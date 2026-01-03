#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# ifdef FT_IRC_DEBUG
  #  define DBG(someCode) do { someCode; } while (0)
# else
  #  define DBG(someCode) do { } while (0)
# endif

# include <string>
# include <vector>
# include "Client.hpp"

class Server;

class Channel
{
	private:
	Server*					_server;
	std::string				_channelname;	//Name of the channel i.e. the topic
	std::vector<Client *>   _members;		//This is a basic map that will contain all the nicknames of the clients who are apart of the channel
	std::vector<int>		_operators;		//Array of fds belonging to which members are operators
	
	//Mode Settings
	bool		_operatorPriv;
	bool		_inviteonly;
	bool		_topicPriv; 
	std::string _password;
	size_t	  	_userlimit; //Immportant as we should dicuss what to do if you set the limit to 2 users and there is 5 who are apart of the channel

	public:
	Channel(Server *server, int fd, std::string name);
	~Channel();
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);

	//Member Functions
	void					channelTopic(std::string newtopic);
	void					AddMember(Client* user);
	void					RemoveMember(std::string username);
	void					RemoveMemberByFd(int fd);
	std::string				getname(void) const;
	size_t		  			getUserlimit(void) const;
	size_t		  			getMembersize(void) const;
	bool					getInviteonly(void) const;
	bool					getTopicpriv(void) const;
	std::string	 			getPassword(void) const;
	bool					isMember(Client* client);
	std::vector<Client *>*	getMembers(void);
	void					printMembers(void);

	//Operator Commands
	bool	IsOperator(int fd);
	void	SetOperator(std::string username, int fd);
	void	UnsetOperator(std::string username, int fd);

	void	kick(std::string username, std::string comments, int fd);
	void	invite(std::string username, int fd);
	void	mode(int fd, std::string param, std::string input);
};

#endif