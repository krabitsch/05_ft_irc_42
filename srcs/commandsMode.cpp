  // Commands_Mode.cpp	 // MODE/INVITE/KICK (subset)
  #include "../includes/Server.hpp"
  #include <cstdlib>
#include <sstream>

  //Mode - Change the Channels Mode
  //Flags
  //- i: Set/remove the invite only 
  //- t: Set/remove the restrictions of the TOPIC command to the channel operators
  //- k: Set/remove the channel key (password) //basically remove the password access
  //- o: Give/take channel operator privilege
  //- l: Set/remove the user limit to channel 

  void Channel::mode(int fd, std::string param, std::string input)//the input is one of the flags 
  {
	//Set certain status in the channel
	if (IsOperator(fd))
	{
	  if (param == "-i")
	  {
		if (_inviteonly == true)
		{
			_inviteonly = false;
			_server->sendNotice(fd, _channelname, "Invite only mode has been removed from the channel");
		}
		else
		{
			_inviteonly = true;
			_server->sendNotice(fd, _channelname, "Invite only mode has been turned on for the channel " + _channelname);
		}
	  }
	  else if (param == "-t")
	  {
		if (_topicPriv == true)
		{
			_server->sendNotice(fd, _channelname, "Topic privilege has been removed from the channel");
			_topicPriv = false;
		}
		else
		{
			_server->sendNotice(fd, _channelname, "Topic privilege has been set for the channel " + _channelname);
			_topicPriv = true; 
		}
	  }
	  else if (param == "-k")
	  {
		if (_password.empty() && !input.empty() || !_password.empty() && !input.empty())
		{
		  _password = input;
		  _server->sendNotice(fd, _channelname, "Channel key has been set/changed for the channel " + _channelname);
		}
		else if (!_password.empty() && input.empty())
		{
		  _password.erase();
		  _server->sendNotice(fd, _channelname, "Channel key has been removed from the channel " + _channelname);
		}
		else 
		{
		  _server->sendNumeric(fd, 814, "", std::vector<std::string>(), "No avaiable password key given!");
		  return ;
		}
	  }
	  else if (param == "-o")
	  {
		if (_operatorPriv == true)
		{
			_server->sendNotice(fd, _channelname, "Operator privilege has been removed from the channel");
			_operatorPriv = false;
		}
		else
		{
			_operatorPriv = true;
			_server->sendNotice(fd, _channelname, "Operator privilege has been set for the channel " + _channelname);
		 }
	  }
	  else if (param == "-l") //takes a string convert it into an int or size_t
	  {
		int num = 0;//Here we would convert the numerical string value into an int

		if (input.empty())
		{
		  _server->sendNumeric(fd, 814, "", std::vector<std::string>(), "No numerical input given!"); 
		  return;
		}

		std::stringstream convert(input); //Converts string to int
		convert >> num;

		//Check Number Validity
		if (num < 0)
		  _server->sendNumeric(fd, 814, "", std::vector<std::string>(), "Invalid numerical input!"); 
		if (num == 0)
		{
		  _userlimit = 0;
		  _server->sendNotice(fd, _channelname, "User limit has been removed for the channel " + _channelname);
		}
		else if (num < _members.size())
		  _server->sendNumeric(fd, 814, "", std::vector<std::string>(), "Too many members are already apart of the channel: Unable to Set Limit!"); //Verify if that is the correct numerical number
		else
		{
		  _userlimit = num;
		  _server->sendNotice(fd, _channelname, "User limit has been set for the channel " + _channelname);
		}
	  }
	  else 
	  {
		//ERR_UNKNOWNMODE 
		_server->sendNumeric(fd, 472, "", std::vector<std::string>(), param + ":is unknown mode char to me");
	  }
	}
	else 
	{
	  //ERR_CHANOPRIVSNEEDED  
	  _server->sendNumeric(fd, 482, "", std::vector<std::string>(),_channelname + " :You're not channel operator");
	}
	return ;
  }

  //Invite - Invite a client of the current channel
  //Step 1:Use either the client nickname or username 
  //Step 2:Enter a loop until the user either accepts the invite or rejects it //Remove the loop and just let them have access to the channel
  //Step 3:If client accepts it add to client that he now has access to the channel, and add the user into the channel

  void Channel::invite(std::string username, int fd)
  {
	if (IsOperator(fd) == true) //checks the user executing the command is an operator
	{
	  int i = 0;
	  while (i < _members.size()) //checks all the members if the channel
	  {
		if (_members[i]->getNickname() == username || _members[i]->getUsername() == username) //compares the user written to possible users
		{
		  //ERR_USERONCHAN 443 //check if correct error code and sentance
		  _server->sendNumeric(fd, 443, "", std::vector<std::string>(), "User is already in the channel");
		  return ;
		} 
		i++;
	  }

	  //Gets the client information from the server
	  Client *client = _server->findClientByNickOrUser(-1, username);
	  if (client == NULL)
	  {
		//ERR_NOSUCHNIC //need to add the number code and sentance
		_server->sendNumeric(fd, 000, "", std::vector<std::string>(), "nick does not exist");

	  }
	  AddMember(client); //adds the client onto the channel list
	  client->AddChannel(_channelname, 'm'); //adds the channel to the clients channels
	  std::string msg = username + " you have been invited to " + _channelname + "channel";
	  client->addNotification(msg, 'i'); //give them a notification that they have been invited
	}
	else 
	{
	  //ERR_NOCHANPRIV
	  _server->sendNumeric(fd, 482, "", std::vector<std::string>(),_channelname + " :You're not channel operator");
	}
  }

  //KICK - Ejecting a client from the channel
  //The Kick interacts with the channel, with no channel you are unable to kick
  //Step 1: First we need if the client & Channel & Operator exists
  //Step 2: Check if they are all apart of the channel and have access to it, 
  //Step 3: Declare in the channel that they are getting kick simple std::cout message
  //Step 4: Remove the user from the channel 

  //Questions to Ask: Should an operator be able to kick another operator?

  void Channel::kick(std::string username, std::string comments, int fd)
  {
	if (IsOperator(fd) == true) //checks the user executing the command is an operator
	{
	  int i = 0;
	  while (i < _members.size()) //checks all the members if the channel
	  {
		if (_members[i]->getNickname() == username || _members[i]->getUsername() == username) //compares the user written to possible users
		{
		  RemoveMember(username); //removes the member from the channel and removes the channel from there channel list
		  _server->sendNotice(fd, _channelname, username + " has been kicked from the channel");
		  return ;
		}
		i++;
	  }
	  //ERR_USERNOTINCHANNEL 441
	  _server->sendNumeric(fd, 441, "", std::vector<std::string>(), username + " " + _channelname +" :They aren't on that channel");
	}
	else 
	{
	  //ERR_CHANOPRIVSNEEDED 482
	  _server->sendNumeric(fd, 482, "", std::vector<std::string>(),_channelname + " :You're not channel operator");
	}
  }
  
