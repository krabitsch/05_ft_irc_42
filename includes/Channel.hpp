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
	Server *_server;
	std::string _channelname; //Name of the channel i.e. the topic
	std::vector<Client *> _members; //This is a basic map that will contain all the nicknames of the clients who are apart of the channel
	std::vector<int> _operators; //Array of fds belonging to which members are operators
	std::string _topic;

	//Mode Settings
	bool		_operatorPriv;
	bool		_inviteonly;
	bool		_topicPriv; 
	std::string _password;
	size_t	  	_userlimit; //Immportant as we should dicuss what to do if you set the limit to 2 users and there is 5 who are apart of the channel

	public:
	Channel(Server *server, int fd, std::string name);
	Channel(Server *server, int fd, std::string name, std::string pass);
	~Channel();
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);

	//Member Functions
	void AddMember(Client* user);
	void RemoveMember(std::string username);
	std::string getname(void) const;
	std::string getTopic(void) const;
	size_t	getUserlimit(void) const;
	size_t	getMembersize(void) const;
	bool 	getInviteonly(void) const;
	bool	getTopicpriv(void) const;
	std::string	getPassword(void) const;
	bool isMember(Client* client);
	std::vector<Client *>*	getMembers(void);
	std::vector<int>*		getOperators(void);
	void 					printMembers(void);
	void					RemoveMemberByFd(int fd);
	void 					rpl_namrepl(int fd);

	//Mode Commands
	int modeI(int fd, std::string param);
	int modeT(int fd, std::string param);
	int modeO(int fd, std::string param, std::string input);
	int modeK(int fd, std::string param, std::string input);
	int modeL(int fd, std::string param, std::string input);

	//Operator Commands
	bool IsOperator(int fd);
	void SetOperator(std::string username, int fd);
	void setTopic(std::string word);
	void setPassword(std::string word);
	void UnsetOperator(std::string username, int fd);

	void kick(std::string username, std::string comments, int fd);
	void invite(std::string username, int fd);
	void mode(int fd, std::vector<std::string> params);
};

#endif
