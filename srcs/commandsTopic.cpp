// Commands_Topic.cpp    // TOPIC
#include "../includes/Server.hpp"


//TOPIC change the channel and either view the current channel that the user is in or display all channels
//Gonna be honest im unsure which one it actually is, looking into it i might of been wrong and it only displays the current channel topic and changes the current channel topic

void Server::topic(std::string channelname, int clientfd)
{

  //Changing Channel 
  //Note: that this should only be executed when the TOPIC command has an additional prameter 
  //Check if the channel exists 
  Channel *channel_type = findChannel(channelname);
  Client *user = findClient(clientfd, NULL); 
  if (channel_type != NULL)
  {
    //Switch channels

    user->setCurrentChannel(channelname);
    return ;
  }

  //Display current Channel
  std::cout << "The Chanel you are currently in: " << user->GetChannel();
}