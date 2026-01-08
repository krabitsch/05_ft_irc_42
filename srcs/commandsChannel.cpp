  // Commands_Channel.cpp  // JOIN/PART/NAMES
  #include "../includes/Server.hpp"

  //Join
  //Switching Channels and joining the one in the prameter 

  //JOIN Needs to be fixed when joining the channel the user isnt added to the channel list of the client

  void Server::join(int fd, std::string channelname, std::string pass)
  {
    Channel *_channel = findChannel(channelname);

    if (_channel == NULL)
    {
      if (!pass.empty())
      {
        Channel _newchannel(this, fd,channelname, pass); //creates the channel with passkey
        _channels.push_back(_newchannel);
      }
      else
      {
        Channel _newchannel(this, fd,channelname); //creates the channel
        _channels.push_back(_newchannel);
      }
      this->sendMessage(fd, _serverName, "JOIN", std::vector<std::string>(1, channelname), "You have created this channel");
    }
    else 
    {
      Client *_client = findClient(fd, "");
      if (_client == NULL)
      {
        //ERR_NOSUCHNICK *Technically this should never happen!
        this->sendNumeric(fd, 401, "", std::vector<std::string>(), ":No such nick");
        return ;
      }

      //Getting Clients Channel List
      std::map<std::string, char>*channelist = _client->GetChannel();

      //Checks if the user has access to the channel already or not if not add it to there channellist
      bool checker = (channelist->find(channelname) != channelist->end());

      //Error Handling
      if (_channel->getInviteonly() == true && checker == false)
      {
        //ERR_INVITEONLYCHAN 473
        this->sendNumeric(fd, 473, "", std::vector<std::string>(), _channel->getname() + "Cannot join channel (+i)"); //You need an invite to join
        return ;
      }
      if (_channel->getUserlimit() == _channel->getMembersize())
      {
        //ERR_CHANNELISFULL 471
        this->sendNumeric(fd, 471, "", std::vector<std::string>(), _channel->getname() + "Cannot join channel (+l)"); //Channel is full
        return ;
      }
      if (!_channel->getPassword().empty() && _channel->getPassword() != pass)
      {
        //ERR_BADCHANNELKEY 475
        this->sendNumeric(fd, 475, "", std::vector<std::string>(), _channel->getname() +":Cannot join channel (+k)"); //Wrong password when joining 
        return ;
      }

      //If no errors were found add the user, display message, if its a new channel add it to there list
      _client->setCurrentChannel(channelname);
      this->sendNotice(fd, channelname, _client->getNickname() + " has joined the channel"); 
      
      if (!checker) //Adds the channel to the client list !isnt is here 
        _channel->AddMember(_client);
    }
  }

  //Part //leaves the channel you are currently on!
  void Server::part(int fd, std::string channelname)
  {
    Client *client = findClient(fd, ""); //Finds the client
    if (client == NULL) //error handling but client does exist
    {
      this->sendNumeric(fd, 401, "", std::vector<std::string>(), "nick does not exist");
      return ;
    }

    Channel *channel_type = findChannel(channelname);
    if (channel_type == NULL) //Checkes if the channel doesnt exist
    {
      this->sendNumeric(fd, 403, "", std::vector<std::string>(), channelname + " :No such channel");
      return ;
    }
    this->sendNotice(fd, channel_type->getname(), client->getNickname() + " has left the channel");
    channel_type->RemoveMember(client->getNickname()); //Removes the member from the channel
    client->RemoveChannel(client->getCurrentChannel()); //Removes the channel from the clients list
    client->setCurrentChannel(""); //Sets there current channel to nothing
    return ;
    
  }
