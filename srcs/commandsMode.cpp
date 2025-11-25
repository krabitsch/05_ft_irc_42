  // Commands_Mode.cpp     // MODE/INVITE/KICK (subset)
  #include "../includes/Server.hpp"
  #include <cstdlib>

  //Mode - Change the Channels Mode
  //Flags
  //- i: Set/remove the invite only 
  //- t: Set/remove the restrictions of the TOPIC command to the channel operators
  //- k: Set/remove the channel key (password) //basically remove the password access
  //- o: Give/take channel operator privilege
  //- l: Set/remove the user limit to channel 

  void Channel::mode(int fd)//the input is one of the flags 
  {
    char temp;//Temp flag
    std::string param; //Temp string param

    //Set certain status in the channel
    if (IsOperator(fd))
    {
      if (temp == 'i')
      {
        if (_inviteonly == true)
            _inviteonly = false;
        else
            _inviteonly = true;
      }
      else if (temp = 't')
      {
        if (_topicPriv == true)
            _topicPriv = false;
        else
            _topicPriv = true;
      }
      else if (temp = 'k')
      {
        if (_password.empty() && !param.empty() || !_password.empty() && !param.empty())
          _password = param;
        else
          _password.erase();
      }
      else if (temp = '0')
      {
        if (_operatorPriv == true)
            _operatorPriv = false;
        else
            _operatorPriv = true;
      }
      else if (temp = 'l') //takes a string convert it into an int or size_t
      {
        int num = 0;//Here we would convert the numerical string value into an int

        if (num < 0)
          std::cerr << "The note set is invalid" << std::endl; //Maybe change this into a throw
        if (num < _members.size())
          std::cout << "They are currently too many members who are apart of the channel" << std::endl;
        else
          _userlimit = num;
      }
    }
    else 
    {
      std::cerr << "You are not an operator!" << std::endl;
    }
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
        if (_members[i].getNickname() == username || _members[i].getUsername() == username) //compares the user written to possible users
        {
          std::cout << "User is already a member of the channel" << std::endl;
          return ;
        } 
        i++;
      }

      //Gets the client information from the server
      Client *client = _server->findClient(-1, username);
      AddMember(*client); //adds the client onto the channel list
      client->AddChannel(_channelname); //adds the channel to the clients channels
      std::string msg = username + " you have been invited to " + _channelname + "channel";
      client->addNofitication(msg, 'i'); //give them a notification that they have been invited
    }
    else 
    {
      std::cerr << "You are not an operator!" << std::endl;
    }
  }

  //KICK - Ejecting a client from the channel
  //The Kick interacts with the channel, with no channel you are unable to kick
  //Step 1: First we need if the client & Channel & Operator exists
  //Step 2: Check if they are all apart of the channel and have access to it, 
  //Step 3: Declare in the channel that they are getting kick simple std::cout message
  //Step 4: Remove the user from the channel 

  //Questions to Ask: Should an operator be able to kick another operator?

  void Channel::kick(std::string username, int fd)
  {
    if (IsOperator(fd) == true) //checks the user executing the command is an operator
    {
      int i = 0;
      while (i < _members.size()) //checks all the members if the channel
      {
        if (_members[i].getNickname() == username || _members[i].getUsername() == username) //compares the user written to possible users
        {
          RemoveMember(username); //removes the member from the channel and removes the channel from there channel list
          //the User should then bekicked from the server 
          return ;
        }
        i++;
      }
      std::cerr << "The user does not exist in " << _channelname << " channel" << std::endl; //error msg
    }
    else 
    {
      std::cerr << "You are not an operator!" << std::endl; //error msg
    }
  }
  
