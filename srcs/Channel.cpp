#include "../includes/Server.hpp"
#include <iostream>

//Constructor 
Channel::Channel(Server *server, int fd, std::string name): _server(server), _channelname(name)
{
	Client *client = _server->findClientByFd(fd);
	if (!client)
		return ;
	client->AddChannel(name, 'o');
	AddMember(client); // AddMember() inserts 'm' into the client map again, so change again below:
	(*client->GetChannel())[name] = 'o';
	client->setCurrentChannel(name);
	_operators.push_back(client->getFd());
	_inviteonly = false;
	_topicPriv = false;
	_password = "";
	_userlimit = 0;
	_operatorPriv = true;
};

//Destructor
Channel::~Channel() {};

//Copy Contructor 
Channel::Channel(const Channel &other): _server(other._server),
	_channelname(other._channelname),
	_members(other._members),
	_operators(other._operators),
	_operatorPriv(other._operatorPriv),
	_inviteonly(other._inviteonly),
	_topicPriv(other._topicPriv),
	_password(other._password),
	_userlimit(other._userlimit) {};

//Copy Assign Operator
Channel &Channel:: operator=(const Channel &other) 
{
	if (this != &other)
	{
		this->_server = other._server;
		this->_channelname = other._channelname;
		this->_members = other._members;
		this->_operators = other._operators;
		this->_operatorPriv = other._operatorPriv;
		this->_inviteonly = other._inviteonly;
		this->_topicPriv = other._topicPriv;
		this->_password = other._password;
		this->_userlimit = other._userlimit;
	}
	return (*this);
};

//Change Topic

void Channel::channelTopic(std::string newtopic)
{
	std::string oldtopic = _channelname;
	_channelname = newtopic; 
	
	size_t i = 0;
	while (i < _members.size())
	{
		_members[i]->setCurrentChannel(newtopic);
		_members[i]->RemoveChannel(oldtopic); 
		if (IsOperator(_members[i]->getFd()) == true)
			_members[i]->AddChannel(_channelname, 'o');
		else
			_members[i]->AddChannel(_channelname, 'm');
		i++;
	}
}


//Add Member
//Step 1: Get client and add it to the map
void Channel::AddMember(Client* user)
{
	if (!user)
		return ;

	for (size_t i = 0; i < _members.size(); i++)
	{
		// check if already in channel (by fd (unique), not username or nickname)
		if (_members[i] && _members[i]->getFd() == user->getFd())
			return;
	}
	/*
	size_t i = 0;
	while (i < _members.size())
	{
		if (_members[i]->getNickname() == user->getNickname() || _members[i]->getUsername() == user->getUsername())
			return ;
		i++;
	}
	*/
	_members.push_back(user);
	user->AddChannel(_channelname, 'm'); // maybe rather do this via Channel constructor and/or let Server decide roles
										 // would overwrite 'o' status here
}

//Remove Member
//Step 1: Check if client exist in the channel or not
//Step 2: Remove client from vector
void Channel::RemoveMember(std::string username)
{
	size_t i = 0;
	while (i < _members.size())
	{
		if (_members[i]->getNickname() == username || _members[i]->getUsername() == username)
		{
			_members[i]->RemoveChannel(_channelname);
			_members[i]->setCurrentChannel(""); //Set the users current channel to blank
			if (IsOperator(_members[i]->getFd()) == true) //Remove the user as an operator
			{
				size_t i = 0;
				while (i < _operators.size())
				{
					if (_operators[i] == _members[i]->getFd())
					{
						_operators.erase(_operators.begin() + i);
						break ;
					}
					i++;
				}
			}
			_members.erase(_members.begin() + i); //removes the user from members
			return ;
		}
		i++;
	}
}

bool Channel::isMember(Client* client)
{
	if (!client)
		return false;
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i]->getFd() == client->getFd())
			return true;
	}
	return false;
}

std::vector<Client *>* Channel::getMembers(void)
{
	return &_members;
}

//Set Operator Privilage
//Step 1: Check if user is already a operator and is already a member
//Step 2: Add member onto the vector 
//Step 3: Set the status in the client class

bool Channel::IsOperator(int fd) //Checks if the user is an operator or not
{
	if (_operatorPriv == true)
	{
		size_t i = 0;
		while (i <  _operators.size())
		{
			if (fd == _operators[i])
				return true;
			i++;
		}
		return false;
	}
	else
		return true;
}

void Channel::SetOperator(std::string username, int fd) //Another Note: This function isnt done we still need to verify that the one executing this comamnd is a user or everyone is an operator
{
	//If the user is an operator
	if (_operatorPriv == true)
	{
		size_t i = 0;
		while (i < _members.size()) //Note: Maybe put this into its own sperate function
		{
			if (_members[i]->getNickname() == username || _members[i]->getUsername() == username)
			{
				if (IsOperator(_members[i]->getFd()) == false)
				{
					std::map<std::string, char> *user_channels = _members[i]->GetChannel();
					(*user_channels)[_channelname] = 'o'; //check if this sets the channel in the client correctly
					_operators.push_back(_members[i]->getFd());
					_server->sendNotice(_members[i]->getFd(), _channelname, "You are now an operator in " + _channelname + " channel");
				}
				else 
				{
					_server->sendNumeric(fd, 443, "", std::vector<std::string>(), "User is already an operator in this channel");
					return ;
				}
			}
			i++;
		}
		_server->sendNumeric(fd, 441, "", std::vector<std::string>(), "User is not in the channel");
	}
	else 
	{
		_server->sendNumeric(fd, 482, "", std::vector<std::string>(), "You are not an operator!");
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
		size_t i = 0;
		while (i < _members.size())
		{
			if (_members[i]->getNickname() == username || _members[i]->getUsername() == username)
			{
				if (IsOperator(_members[i]->getFd()) == true)
				{
					int clientfd = _members[i]->getFd();
					std::map<std::string, char> *user_channels = _members[i]->GetChannel();
					(*user_channels)[_channelname] = 'm'; //Set there status back to member


					//Creates the new list of operators without the unset member
					std::vector<int> newoperators;
					size_t k = 0;
					while (k < _operators.size())
					{
						if (_operators[k] != clientfd)
							newoperators.push_back(_operators[k]);
						k++;
					}
					_operators = newoperators;
					_server->sendNotice(clientfd, _channelname, "You are no longer an operator in " + _channelname + " channel");
				}
				else 
				{
					_server->sendNumeric(fd, 443, "", std::vector<std::string>(), "User is already not an operator in this channel");
					return ;
				}
			}
			i++;
		}
	}
	else 
	{
		_server->sendNumeric(fd, 482, "", std::vector<std::string>(), "You are not an operator!");
	}
}

void Channel::printMembers(void) //Prints all the members in the channel
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		std::cout << "Member " << i + 1 << ": " << _members[i]->getNickname() << " (" << _members[i]->getUsername() << ")" << std::endl;
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

bool Channel::getTopicpriv(void) const
{
	return (_topicPriv);
}

std::string	Channel::getPassword(void) const
{
	return (_password);
}