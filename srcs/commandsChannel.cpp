  // Commands_Channel.cpp  // JOIN/PART/NAMES
  #include "../includes/Server.hpp"

  //Join
  //Switching Channels and joining the one in the prameter 

  void Server::join(int fd, std::string channelname)
  {
    if (findChannel(channelname) == NULL) //if the channel does not exist then create it and automatically join it
    {
      Channel channel(_serverAdd, fd,channelname);
      _channels.push_back(channel);
    }
    else //Join the channel if it exists //Note: that we should check if the user even has access to it and whether or not it is private 
    {
    
    }

  }

  //Part //We dont technically need this! decide whether or not we want to implement this feature!
  //leaves the channel you are currently on!
  void Server::part(int fd)
  {


  }

  //Names
  //Also unsure what this does? 

