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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Invite only mode has been removed from the channel");
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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Invite only mode has been turned on for " + _channelname);
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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Topic privilege has been removed from " + _channelname);
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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Topic privilege has been set for " + _channelname);
      }
      else
        _server->sendNumeric(fd, 467, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, _channelname), "Channel key already set");
      return (1);
    }
    return (0);
  }

  int Channel::modeO(int fd, std::string param, std::string input)
  {
    if (input.empty() && (param == "+o" || param == "-o"))
    {
      _server->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "MODE"),
					"Not enough parameters");
      return (1);
    }
    if (param == "-o") //false
    {
      Client *client = _server->findClientByNickOrUser(-1, input);
      if (client == NULL)
      {
        _server->sendNumeric(fd, 401, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, input), "No such nick/channel");
        return (1);
      }

      if (IsOperator(client->getFd()) == true)
      {
        UnsetOperator(input, fd);
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), input + " is no longer an operator in " + _channelname);
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
      
      if (IsOperator(client->getFd()) == false)
      {
        SetOperator(input, fd);
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), input + " is now an operator in " + _channelname);
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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByFd(fd)->getUsername(), _server->findClientByFd(fd)->getUsername(), "Channel key has been removed from the channel " + _channelname);
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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), "Channel key has been set/changed for the channel " + _channelname);
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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), "User limit has been removed from " + _channelname);
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
        _server->broadcastMessage("MODE", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), "User limit has been set for " + _channelname);
      }
      return (1);
    }
    return (0);
  }

  void Channel::mode(int fd, std::vector<std::string> param)//the input is one of the flags 
  {
    //Set certain status in the channel
    if (IsOperator(fd))
    {
      if (param.size() < 2)
      {
        //ERR_NEEDMOREPARAMS 
        _server->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "MODE"),"Not enough parameters");
          return ;
      }
      size_t count = 1;
      size_t has_o = 0;
      int has_l = 0;
      int status = 0;

      
      if (param[1].find("l") != std::string::npos)
      {
        count++;
        has_l = 1;
      }
      if (param[1].find("k") != std::string::npos || param[1].find("o") != std::string::npos)
        count++;
      if (param[1][0] == '+')
        status = 1;
      else if (param[1][0] == '-')
        status = 0;
      if (param[1].find("o") != std::string::npos)
        has_o = 1;
      if ((status == 1 && count != param.size() - 1) || (status == 0 && param.size() - 1 >= 2 + has_o))
      {
        //ERR_NEEDMOREPARAMS 
        _server->sendNumeric(fd, 461, "*", std::vector<std::string>(1, "MODE"),
          "Not enough parameters");
            return ;
      }

      for (size_t i = 1; i < param[1].length(); i++)
      {
        if(param[1][i] == 't'){
          if (status == 1)
            modeT(fd, "+t");
          else
            modeT(fd, "-t");
        }
        else if(param[1][i] == 'i'){
          if (status == 1)
            modeI(fd, "+i");
          else
            modeI(fd, "-i");
        }
        else if(param[1][i] == 'l'){
          if (status == 1)
            modeL(fd, "+l", param[2]);
          else
            modeL(fd, "-l", "");
        }
        else if(param[1][i] == 'o'){
          if (status == 1)
            modeO(fd, "+o", param[2 + has_l]);
          else
            modeO(fd, "-o", param[2 + has_l]);
        }
        else if(param[1][i] == 'k'){
          if (status == 1)
            modeK(fd, "+k", param[2 + has_l]);
          else
            modeK(fd, "-k", "");
        }
        else
          //ERR_UNKNOWNMODE 
          _server->sendNumeric(fd, 472, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, param[2]), ":is unknown mode char to me");
      }
    }
    else 
    { //ERR_CHANOPRIVSNEEDED  
      _server->sendNumeric(fd, 482, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, param[2]), "You're not a channel operator");
    }
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
      _server->sendNumeric(fd, 341, _server->findClientByFd(fd)->getNickname(), std::vector<std::string>(1, username), _channelname + " :You have invited " + username + " to " + _channelname);
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
    if (IsOperator(fd) == true) //checks the user executing the command is an operator
    {
      size_t i = 0;
      while (i < _members.size()) //checks all the members if the channel
      {
        if (_members[i]->getNickname() == username) //compares the user written to possible users
        {
          if (!comments.empty())
            _server->broadcastMessage("KICK", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), username + " has been kicked from " + _channelname + " (" + comments + ")");
          else
            _server->broadcastMessage("KICK", _channelname, _server->findClientByNickOrUser(fd, "")->getNickname(), _server->findClientByNickOrUser(fd, "")->getUsername(), username + " has been kicked from " + _channelname);
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