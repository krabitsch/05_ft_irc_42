#include "../includes/Channel.hpp"
#include <string_view>

Channel::Channel(std::string name): channel_name(name) 
{
	//Set the user who made the channel as its first memeber with the operator status
	

};

Channel::~Channel() {};
Channel::Channel(const Channel &type) {};
Channel &Channel:: operator=(const Channel &type1) {};

//Add Member
//Step 1: Get client and add it to the map
void Channel::AddMember(Client user)
{
	members.push_back(user);
}

//Remove Member
//Step 1: Check if client exist in the channel or not
//Step 2: Remove client from vector
void Channel::RemoveMember(std::string username)
{
	int i = 0;
	while (i < members.size())
	{
		if (members[i].GetNickname() == username || members[i].GetUsername() == username)
		{
			std::map<std::string, char> *user_channels = members[i].GetChannel(); //Gets the channel array
			user_channels->erase(channel_name); //removes the channel
			members[i].SetChannel(user_channels); //Inserts new channel array
			members.erase(members.begin() + i); //removes the user from members
			//Kicks them idk how to do that yet XD
		}
		i++;
	}
	std::cerr << "User does not exist in this channel" << std::endl;
}

//Set Operator Privilage
//Step 1: Check if user is already a operator and is already a member
//Step 2: Add member onto the vector 
//Step 3: Set the status in the client class

bool Channel::IsOperator(int fd)
{
	int i = 0;
	while (i <  operators.size())
	{
		if (fd == operators[i])
			return true;
		i++;
	}
	return false;
}

void Channel::SetOperator(std::string username)
{
	//If the user is an operator
	int i = 0;
	while (i < members.size())
	{
		if (members[i].GetNickname() == username || members[i].GetUsername() == username)
		{
			if (IsOperator(members[i].GetFdSocket()) == false)
			{
				
				std::map<std::string, char> *user_channels = members[i].GetChannel();
				(*user_channels)[channel_name] = 'o';
				operators.push_back(members[i].GetFdSocket());
				std::cout << "User " << username << " is now an operator" << std::endl;
			}
			else 
			{
				std::cout << "User is already an operator for this channel" << std::endl;
				return ;
			}
		}
		i++;
	}
	std::cerr << "User does not exist in this channel" << std::endl;
}

//Unset Operator Privilage
//Step 1: Check if user is already a operator and is already a member
//Step 2: remove member in the vector 
//Step 3: Set the status in the client class

std::string Channel::getname(void)
{
	return (channel_name);
}