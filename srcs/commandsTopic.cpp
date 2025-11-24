// Commands_Topic.cpp    // TOPIC
#include "../includes/Server.hpp"


//TOPIC change the channel and either view the current channel that the user is in or display all channels
//Gonna be honest im unsure which one it actually is

void Server::topic(std::string channelname, int clientfd)
{

  //Changing Channel 
  //Note: that this should only be executed when the TOPIC command has an additional prameter 
  //Check if the channel exists 
  Channel *channel_type = findChannel(channelname);
  if (channel_type != NULL)
  {
    //Switch channels

    return ;
  }

  //Display current Channel or Avaiable channels im unsure about this 
  Client *user = findClient(clientfd);


}