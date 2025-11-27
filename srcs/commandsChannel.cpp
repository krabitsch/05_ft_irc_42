  // Commands_Channel.cpp  // JOIN/PART/NAMES
  #include "../includes/Server.hpp"

  //Join
  //Switching Channels and joining the one in the prameter 

  void Server::join(int fd, std::string channelname)
  {
    Channel *_channel = findChannel(channelname);

    if (_channel == NULL)
    {
      Channel _newchannel(_serverAdd, fd,channelname); //creates the channel
      _channels.push_back(_newchannel);
    }
    else 
    {
      Client *_client = findClient(fd, NULL);
      std::map<std::string, char>*channelist = _client->GetChannel();

      //Checks if the user has access to the channel already or not if not add it to there channellist
      bool checker = false;
      std::map<std::string, char>::iterator it = channelist->begin();
      while (it != channelist->end())
      {
        if (it->first == channelname)
          checker = true;
        it++;
      }

      if (_channel->getInviteonly() == true && checker == false)
      {
        std::cerr << "This channel is invite only!" << std::endl;
        return ;
      }
      if (_channel->getUserlimit() == _channel->getMembersize())
      {
        std::cout << "Unable to join due to channel reaching member limmit" << std::endl;
        return ;
      }
      _client->setCurrentChannel(channelname);
      std::cout << _client->getUsername() << " switched to " << channelname << std::endl;
      if (checker == false) //Adds the channel to the client list
        _client->AddChannel(channelname);
    }

  }

  //Part //We dont technically need this! decide whether or not we want to implement this feature!
  //leaves the channel you are currently on!
  void Server::part(int fd)
  {
  }

  //Names
  //Also unsure what this does? 

