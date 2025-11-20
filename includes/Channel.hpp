#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

class Channel
{
	private:
	std::string name;
	std::map<int, Client> members; //This is a basic map that will contain all the nicknames of the clients who are apart of the channel
	std::vector<std::string> operators; //String of names belong to which members are operators

	public:
	Channel();
	~Channel();
	Channel(const Channel &type);
	Channel &operator=(const Channel &type1);

	void AddMember();
	void RemoveMember();

	//All Mode Commands will be inside the channel class
};