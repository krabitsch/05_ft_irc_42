#include "../includes/Server.hpp"
#include <iostream>

//Constructor 
Channel::Channel(Server *server, int fd, std::string name): _channelname(name), _server(server) 
{
	Client *client = _server->findClient(fd, NULL);
	client->AddChannel(name);
	AddMember(*client);
	_operators.push_back(client->getFd());
};

//Destructor
Channel::~Channel() {};

//Copy Contructor 
Channel::Channel(const Channel &other) {};

//Copy Assign Operator
Channel &Channel:: operator=(const Channel &other) 
{
	if (this != &other)
	{ //Need to still add this
	}
	return (*this);
};

//Add Member
//Step 1: Get client and add it to the map
void Channel::AddMember(Client user)
{
	_members.push_back(user);
}

//Remove Member
//Step 1: Check if client exist in the channel or not
//Step 2: Remove client from vector
void Channel::RemoveMember(std::string username)
{
	int i = 0;
	while (i < _members.size())
	{
		if (_members[i].getNickname() == username || _members[i].getUsername() == username)
		{
			std::map<std::string, char> *user_channels = _members[i].GetChannel(); //Gets the channel array
			user_channels->erase(_channelname); //removes the channel
			_members[i].setChannel(user_channels); //Inserts new channel array
			_members[i].setCurrentChannel("lobby"); //Set the users current channel to the general or lobby
			_members.erase(_members.begin() + i); //removes the user from members
			return ;
		}
		i++;
	}
}

//Set Operator Privilage
//Step 1: Check if user is already a operator and is already a member
//Step 2: Add member onto the vector 
//Step 3: Set the status in the client class

bool Channel::IsOperator(int fd) //Checks if the user is an operator or not
{
	if (_operatorPriv == true)
	{
		int i = 0;
		while (i <  _operators.size())
		{
			if (fd == _operators[i])
				return true;
			i++;
		}
		return false;
	}
	else
		return true; //We can change this if needed
}

void Channel::SetOperator(std::string username, int fd) //Another Note: This function isnt done we still need to verify that the one executing this comamnd is a user or everyone is an operator
{
	//If the user is an operator
	if (_operatorPriv == true)
	{
		int i = 0;
		while (i < _members.size()) //Note: Maybe put this into its own sperate function
		{
			if (_members[i].getNickname() == username || _members[i].getUsername() == username)
			{
				if (IsOperator(_members[i].getFd()) == false)
				{
					
					std::map<std::string, char> *user_channels = _members[i].GetChannel();
					(*user_channels)[_channelname] = 'o'; //check if this sets the channel in the client correctly
					_operators.push_back(_members[i].getFd());
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
	else 
	{
		std::cout << "Operator privilege is turned off!" << std::endl;
	}
}

//Unset Operator Privilage
//Step 1: Check if user is already a operator and is already a member
//Step 2: remove member in the vector 
//Step 3: Set the status in the client class

void Channel::UnsetOperator(std::string username, int fd)
{
	if (IsOperator(fd) == true) //Checks if the user is an operator themselves 
	{
		//Unset the operator 
		int i = 0;
		while (i < _members.size())
		{
			if (_members[i].getNickname() == username || _members[i].getUsername() == username)
			{
				if (IsOperator(_members[i].getFd()) == true)
				{
					int clientfd = _members[i].getFd();
					std::map<std::string, char> *user_channels = _members[i].GetChannel();
					(*user_channels)[_channelname] = 'm'; //Set there status back to member

					//Creates the new list of operators without the unset member
					std::vector<int> newoperators;
					int k = 0;
					while (k < _operators.size())
					{
						if (_operators[i] != clientfd)
							newoperators.push_back(_operators[i]);
						k++;
					}
					_operators = newoperators;

					std::cout << "User " << username << " is no longer an operator" << std::endl;
				}
				else 
				{
					std::cout << "User is already not an operator in this channel" << std::endl;
					return ;
				}
			}
			i++;
		}

	}
	else 
	{
		std::cout << "You are not an operator in this channel!"	<< std::endl;
	}
}

//Getters
std::string Channel::getname(void) const
{
	return (_channelname);
}

size_t Channel::getUserlimit(void) const
{
	return (_userlimit);
}

size_t Channel::getMembersize(void) const
{
	return (_members.size());
}

bool Channel::getInviteonly(void) const
{
	return (_inviteonly);
}