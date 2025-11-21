#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "Client.hpp"

class Channel
{
	private:
	std::string channel_name; //Name of the channel i.e. the topic
	std::vector<Client> members; //This is a basic map that will contain all the nicknames of the clients who are apart of the channel
	std::vector<int> operators; //Array of fds belonging to which members are operators

	public:
	Channel(std::string name);
	~Channel();
	Channel(const Channel &type);
	Channel &operator=(const Channel &type1);

	//Member Functions
	void AddMember(Client user);
	void RemoveMember(std::string username);

	std::string getname(void);
	bool IsOperator(int fd);

	//All Mode Commands will be inside the channel class
	void SetOperator(std::string username);
	void UnsetOperator(std::string username);
};