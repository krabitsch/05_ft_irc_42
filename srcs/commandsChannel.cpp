// Commands_Channel.cpp  // JOIN/PART/NAMES
#include "../includes/Server.hpp"

//Join
//Switching Channels and joining the one in the prameter 

  //JOIN Needs to be fixed when joining the channel the user isnt added to the channel list of the client

  //Rpl_Namreply 353: used when the user joins the channel!
  void Channel::rpl_namrepl(int fd)
  {
    std::string names;
    
    size_t i = 0;
    while (i < _members.size())
    {
      if (i > 0)
        names += " ";
      if (IsOperator(_members[i]->getFd()) == true)
        names += "@";
      names += _members[i]->getNickname();
      i++;
    }
    
    // Send RPL_NAMREPLY (353) only to the joining user
    std::vector<std::string> params;
    params.push_back("=");
    params.push_back(this->getname());
    Client *client = this->_server->findClientByFd(fd);
    this->_server->sendNumeric(fd, 353, client->getNickname(), params, names);
    
    // Send RPL_ENDOFNAMES (366) only to the joining user
    std::vector<std::string> endParams;
    endParams.push_back(this->getname());
    this->_server->sendNumeric(fd, 366, client->getNickname(), endParams, "End of NAMES list");
  }


  void Server::join(int fd, std::string channelname, std::string pass)
  {
    Channel *_channel = findChannel(channelname);
    std::vector<std::string> params;

    if (_channel == NULL)
    {
      if (!pass.empty())
      {
        Channel *newchannel = new Channel(this, fd, channelname, pass); //creates the channel with passkey
        _channels.push_back(newchannel);
      }
      else
      {
        Channel *newchannel = new Channel(this, fd, channelname, pass); //creates the channel
        _channels.push_back(newchannel);
      }
      this->sendMessage(fd, this->findClientByFd(fd)->getNickname(), "JOIN", std::vector<std::string>(1, channelname), "You have created this channel");
    }
    else 
    {
      params.push_back(_channel->getname());
      Client *_client = findClientByNickOrUser(fd, "");
      if (_client == NULL)
      {
        //ERR_NOSUCHNICK *Technically this should never happen!
        this->sendNumeric(fd, 401, _client->getNickname(), params, ":No such nick");
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
        this->sendNumeric(fd, 473, _client->getNickname(), params, " Cannot join channel (+i)"); //You need an invite to join
        return ;
      }
      if (_channel->getUserlimit() != 0 && _channel->getUserlimit() == _channel->getMembersize())
      {
        //ERR_CHANNELISFULL 471
        this->sendNumeric(fd, 471, _client->getNickname(), params, " Cannot join channel (+l)"); //Channel is full
        return ;
      }
      if (!_channel->getPassword().empty() && _channel->getPassword() != pass)
      {
        //ERR_BADCHANNELKEY 475
        this->sendNumeric(fd, 475, _client->getNickname(), params," Cannot join channel (+k)"); //Wrong password when joining 
        return ;
      }

      //If no errors were found add the user, display message, if its a new channel add it to there list
      _client->setCurrentChannel(channelname);
      this->sendNotice(_client->getFd(), channelname, _client->getNickname() + " has joined the channel");
      this->broadcastMessage("JOIN", channelname, _client->getNickname(), _client->getUsername(), _channel->getTopic());
      if (!checker) //Adds the channel to the client list !isnt is here 
        _channel->AddMember(_client);

      //RPL_NAMREPLY 353
      _channel->rpl_namrepl(fd);

      //Topic Reply
      if (_channel->getTopic().empty())
      { //RPL_NOTOPIC 331
        this->sendNumeric(fd, 331, _client->getNickname(), params, "No topic is set for this channel");
      }
      else
      { //RPL_TOPIC 332
        this->sendNumeric(fd, 332, _client->getNickname(), params, _channel->getTopic());
      }
    }
  }

  //Part //leaves the channel you are currently on!
  void Server::part(int fd, std::string channelname)
  {
    Client *client = findClientByNickOrUser(fd, ""); //Finds the client
    if (client == NULL) //error handling but client does exist
    {
      this->sendNumeric(fd, 401, client->getNickname(), std::vector<std::string>(1, channelname), "nick does not exist");
      return ;
    }

    Channel *channel_type = findChannel(channelname);
    if (channel_type == NULL) //Checkes if the channel doesnt exist
    {
      this->sendNumeric(fd, 403, client->getNickname(), std::vector<std::string>(1, channelname), channelname + " :No such channel");
      return ;
    }
    this->broadcastMessage("PART", channelname, client->getNickname(), client->getUsername(), client->getNickname() + " has left the channel");
    channel_type->RemoveMember(client->getNickname()); //Removes the member from the channel
    if (client->getCurrentChannel() == channelname)
        client->RemoveChannel(channelname); //Removes the channel from the clients list
    client->setCurrentChannel(""); //Sets there current channel to nothing
  }
