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
    else //Join the channel if it exists //Note: that we should check if the user even has access to it and whether or not it is private 
    {
      Client *_client = findClient(fd, NULL);
      
      bool checker = false;
      std::map<std::string, char>*channelist = _client->GetChannel();
      std::map<std::string, char>::iterator it = channelist->begin();
      while (it != channelist->end())
      {
        if (it->first == channelname)
          checker = true;
        it++;
      }
      if (checker == false)
      {
        std::cerr << "You are not apart of this channel" << std::endl;
        return ;
      }
      _client->setCurrentChannel(channelname);
      std::cout << _client->getUsername() << " switch to " << channelname << std::endl;
    }

  }

  //Part //We dont technically need this! decide whether or not we want to implement this feature!
  //leaves the channel you are currently on!
  void Server::part(int fd)
  {


  }

  //Names
  //Also unsure what this does? 

