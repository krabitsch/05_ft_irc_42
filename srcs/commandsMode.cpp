  // Commands_Mode.cpp	 // MODE/INVITE/KICK (subset)
  #include "../includes/Server.hpp"
#include <cstddef>
  #include <cstdlib>
  #include <sstream>
  #include <sys/select.h>

  //Mode - Change the Channels Mode
  //Flags
  //- i: Set/remove the invite only 
  //- t: Set/remove the restrictions of the TOPIC command to the channel operators
  //- k: Set/remove the channel key (password) //basically remove the password access
  //- o: Give/take channel operator privilege
  //- l: Set/remove the user limit to channel 

  int Channel::modeI(int fd, std::string param)
  {

    if (param == "-i") //false
    {
      if (_inviteonly == true)
      {
        _inviteonly = false;
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Invite only mode has been removed from the channel\n");
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    else if (param == "+i") //true
    {
      if (_inviteonly == false)
      {
        _inviteonly = true;
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Invite only mode has been turned on for " + _channelname + "\n");
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    return (0);
  }

  int Channel::modeT(int fd, std::string param)
  {
    if (param == "-t") //false
    {
      if (_topicPriv == true)
      {
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Topic privilege has been removed from " + _channelname + "\n");
        _topicPriv = false;
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    else if (param == "+t") //true
    {
      if (_topicPriv == false)
      {
        _topicPriv = true;
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Topic privilege has been set for " + _channelname + "\n");
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    return (0);
  }

  int Channel::modeO(int fd, std::string param, std::string input)
  {
    if (param == "-o") //false
    {
      Client *client = _server->findClientByNickOrUser(-1, input);
      if (client == NULL)
      {
        _server->sendNumeric(fd, 401, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, input), "No such nick/channel");
        return (1);
      }

      if (IsOperator(client->getFd()) == false)
      {
        UnsetOperator(input, fd);
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), input + " is no longer an operator in " + _channelname + "\n");
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    else if (param == "+o") //true
    {
      Client *client = _server->findClientByNickOrUser(-1, input);      
      if (client == NULL)
      {
        _server->sendNumeric(fd, 401, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, input), "No such nick/channel");
        return (1);
      }
      
      
      if (IsOperator(client->getFd()) == true)
      {
        SetOperator(input, fd);
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), input + " is now an operator in " + _channelname + "\n");
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    return (0);
  }

  int Channel::modeK(int fd, std::string param, std::string input)
  {
    if (param == "-k") //Unset password
    {
      if (!_password.empty())
      {
        _password.erase();
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Channel key has been removed from the channel " + _channelname + "\n");
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    else if (param == "+k") //Set password
    {
      if ((_password.empty() && !input.empty()) || (!_password.empty() && !input.empty()))
      {
        _password = input;
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), "Channel key has been set/changed for the channel " + _channelname + "\n");
      }
      else 
      { //Custom error for empty input
        _server->sendNumeric(fd, 814, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "No available password key given!");
      }
      return (1);
    }
    return (0);
  }

  int Channel::modeL(int fd, std::string param, std::string input)
  {
    if (param == "-l") //removes user limit
    {
      if (_userlimit > 0)
      {
        _userlimit = 0;
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), "User limit has been removed from " + _channelname + "\n");
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    else if (param == "+l") //sets a user limit
    {
      size_t num = 0;//Here we would convert the numerical string value into an int

      if (input.empty())
      { //Custom error for empty input
        _server->sendNumeric(fd, 814, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "No numerical input given!"); 
        return (1);
      }

      std::stringstream convert(input); //Converts string to int
      convert >> num;

      //Check Number Validity
      if (num <= 0)
        _server->sendNumeric(fd, 814, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Invalid numerical input!"); 
      else if (num < _members.size())
        _server->sendNumeric(fd, 814, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Too many members are already apart of the channel: Unable to Set Limit!"); //Verify if that is the correct numerical number
      else
      {
        _userlimit = num;
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), "User limit has been set for " + _channelname + "\n");
      }
      return (1);
    }
    return (0);
  }

  void Channel::mode(int fd, std::string param, std::string input)//the input is one of the flags 
  {
    //Set certain status in the channel
    if (IsOperator(fd))
    {
      if (modeI(fd, param) == 1)
        return ;
      if (modeT(fd, param) == 1)
        return ;
      if (modeO(fd, param, input) == 1)
        return ;
      if (modeK(fd, param, input) == 1)
        return ;
      if (modeL(fd, param, input) == 1)
        return ;

     //ERR_UNKNOWNMODE 
      _server->sendNumeric(fd, 472, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, param), ":is unknown mode char to me");
    }
    else 
    { //ERR_CHANOPRIVSNEEDED  
      _server->sendNumeric(fd, 482, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, param), "You're not a channel operator");
    }
    return ;
  }

  //Invite - Invite a client of the current channel

  void Channel::invite(std::string username, int fd)
  {
    if (IsOperator(fd) == true) //checks the user executing the command is an operator
    {
      size_t i = 0;
      while (i < _members.size()) //checks all the members if the channel
      {
        if (_members[i]->getNickname() == username) //compares the user written to possible users
        {
          //ERR_USERONCHAN 443 //check if correct error code and sentance
          _server->sendNumeric(fd, 443, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), username + " " + _channelname + " :is already on channel");
          return ;
        } 
        i++;
      }

      //Gets the client information from the server
      Client *client = _server->findClientByNickOrUser(-1, username);
      if (client == NULL)
      {
        //ERR_NOSUCHNIC //need to add the number code and sentance
        _server->sendNumeric(fd, 000, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "nick does not exist");
        return ;
      }
      AddMember(client); //adds the client onto the channel list
      client->AddChannel(_channelname, 'm'); //adds the channel to the clients channels
      std::string msg = "You have been invited to " + _channelname;
      _server->sendNotice(client->getFd(), client->getNickname(), msg); //sends the notice to the client
    }
    else 
    {
      //ERR_NOCHANPRIV
      _server->sendNumeric(fd, 482, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "You're not channel operator");
    }
  }

  //KICK - Ejecting a client from the channel

  void Channel::kick(std::string username, std::string comments, int fd)
  {
    (void)comments;

    if (IsOperator(fd) == true) //checks the user executing the command is an operator
    {
      size_t i = 0;
      while (i < _members.size()) //checks all the members if the channel
      {
        if (_members[i]->getNickname() == username) //compares the user written to possible users
        {
          _server->broadcastMessage("KICK", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), username + " has been kicked from " + _channelname + "\n");
          RemoveMember(username); //removes the member from the channel and removes the channel from there channel list
          return ;
        }
        i++;
      }
      //ERR_USERNOTINCHANNEL 441
      _server->sendNumeric(fd, 441, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), username + " " + _channelname +" :They aren't on that channel");
    }
    else 
    {
      //ERR_CHANOPRIVSNEEDED 482
      _server->sendNumeric(fd, 482, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "You're not a channel operator");
    }
  }