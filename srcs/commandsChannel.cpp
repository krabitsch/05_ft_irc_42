  // Commands_Channel.cpp  // JOIN/PART/NAMES
  #include "../includes/Server.hpp"

  //Join
  //Switching Channels and joining the one in the prameter 

  void Server::join(int fd, std::string channelname, std::string pass)
  {
    Channel *_channel = findChannel(channelname);

    if (_channel == NULL)
    {
      Channel _newchannel(this, fd,channelname); //creates the channel
      _channels.push_back(_newchannel);
      this->sendMessage(fd, _serverName, "JOIN", std::vector<std::string>(1, channelname), "has been created!");
    }
    else 
    {
      Client *_client = findClient(fd, "");
      if (_client == NULL)
        return ;

      //Getting Clients Channel List
      std::map<std::string, char>*channelist = _client->GetChannel();

      //Checks if the user has access to the channel already or not if not add it to there channellist
      bool checker = (channelist->find(channelname) != channelist->end());

      //Error Handling
      if (_channel->getInviteonly() == true && checker == false)
      {
        this->sendNumeric(fd, 473, "", std::vector<std::string>(), "Cannot join channel (+i)"); //You need an invite to join
        return ;
      }
      if (_channel->getUserlimit() == _channel->getMembersize())
      {
        this->sendNumeric(fd, 471, "", std::vector<std::string>(), "Cannot join channel (+l)"); //Channel is full
        return ;
      }
      if (!_channel->getPassword().empty() && _channel->getPassword() != pass)
      {
        this->sendNumeric(fd, 476, "", std::vector<std::string>(), "Wrong channel key"); //Wrong password when joining 
        return ;
      }

      //If no errors were found add the user, display message, if its a new channel add it to there list
      _client->setCurrentChannel(channelname);

      if (!_client->getNickname().empty()) //Joins the channel AL: Delete the else if nickname is must be set in the begining 
          this->sendNotice(fd, channelname, _client->getNickname() + " has joined the channel");
      else
          this->sendNotice(fd, channelname, _client->getUsername() + " has joined the channel");

      if (!checker) //Adds the channel to the client list
        _channel->AddMember(_client);
    }
  }

  //Part //We dont technically need this! decide whether or not we want to implement this feature!
  //leaves the channel you are currently on!
  void Server::part(int fd)
  {
    
  }
