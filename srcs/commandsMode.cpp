  // Commands_Mode.cpp     // MODE/INVITE/KICK (subset)
  #include "../includes/Server.hpp"

  //Mode - Change the Channels Mode
  //Flags
  //- i: Set/remove the invite only 
  //- t: Set/remove the restrictions of the TOPIC command to the channel operators
  //- k: Set/remove the channel key (password) //basically remove the password access
  //- o: Give/take channel operator privilege
  //- l: Set/remove the user limit to channel 

  void Channel::mode()//the input is one of the flags 
  {
    //Set certain status in the channel



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
      while (i < members.size()) //checks all the members if the channel
      {
        if (members[i].GetNickname() == username || members[i].GetUsername() == username) //compares the user written to possible users
        {
          std::cout << "User is already a member of the channel" << std::endl;
          return ;
        } 
        i++;
      }

      //Gets the client information from the server
      Server server; //remove this and find out how to pass around the server of at least call for the server
      Client *client = server.findClient(-1, username);
      AddMember(*client); //adds the client onto the channel list
      client->AddChannel(channel_name); //adds the channel to the clients channels
      
      std::string msg = username + " you have been invited to " + channel_name + "channel";
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
      while (i < members.size()) //checks all the members if the channel
      {
        if (members[i].GetNickname() == username || members[i].GetUsername() == username) //compares the user written to possible users
        {
          RemoveMember(username); //removes the member from the channel and removes the channel from there channel list
          return ;
        }
        i++;
      }
      std::cerr << "The user does not exist in " << channel_name << " channel" << std::endl; //error msg
    }
    else 
    {
      std::cerr << "You are not an operator!" << std::endl; //error msg
    }
  }
  
