#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
	private:
	std::string channel_name; //Name of the channel i.e. the topic
	std::vector<Client> members; //This is a basic map that will contain all the nicknames of the clients who are apart of the channel
	std::vector<int> operators; //Array of fds belonging to which members are operators
	
	//Mode Settings
	bool operatorPriv;
	bool inviteonly;
	bool topicPriv; //Im unsure for this one its mode -t 
	std::string password;
	size_t userlimit; //Immportant as we should dicuss what to do if you set the limit to 2 users and there is 5 who are apart of the channel

	public:
	Channel(int fd, std::string name);
	~Channel();
	Channel(const Channel &type);
	Channel &operator=(const Channel &type1);

	//Member Functions
	void AddMember(Client user);
	void RemoveMember(std::string username);
	std::string getname(void);

	//Operator Commands
	bool IsOperator(int fd);
	void SetOperator(std::string username, int fd);
	void UnsetOperator(std::string username, int fd);

	void kick(std::string username, int fd);
	void invite(std::string username, int fd);
	void mode();
};