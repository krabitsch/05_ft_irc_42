#include <string>
#include <vector>
#include "Client.hpp"

class Server;

class Channel
{
	private:
	Server *_server;
	std::string _channelname; //Name of the channel i.e. the topic
	std::vector<Client> _members; //This is a basic map that will contain all the nicknames of the clients who are apart of the channel
	std::vector<int> _operators; //Array of fds belonging to which members are operators
	
	//Mode Settings
	bool _operatorPriv;
	bool _inviteonly;
	bool _topicPriv; //Im unsure for this one its mode -t 
	std::string _password;
	size_t _userlimit; //Immportant as we should dicuss what to do if you set the limit to 2 users and there is 5 who are apart of the channel

	public:
	Channel(Server *server, int fd, std::string name);
	~Channel();
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);

	//Member Functions
	void AddMember(Client user);
	void RemoveMember(std::string username);
	std::string getname(void) const;
	size_t	getUserlimit(void) const;
	size_t	getMembersize(void) const;

	//Operator Commands
	bool IsOperator(int fd);
	void SetOperator(std::string username, int fd);
	void UnsetOperator(std::string username, int fd);

	void kick(std::string username, int fd);
	void invite(std::string username, int fd);
	void mode(int fd);
};