// Commands_Topic.cpp    // TOPIC
#include "../includes/Server.hpp"


//TOPIC change the channel and either view the current channel that the user is in or display all channels
//Gonna be honest im unsure which one it actually is, looking into it i might of been wrong and it only displays the current channel topic and changes the current channel topic

void Server::topic(std::string channelname, int clientfd)
{
  //Changing Channel 
  //Note: that this should only be executed when the TOPIC command has an additional prameter 
  //Check if the channel exists 
  Client *user = findClient(clientfd, ""); 

  if (channelname.empty())
  {
    std::string currentchannel = user->getCurrentChannel();
    this->sendNotice(clientfd, currentchannel, "Current channel topic is: " + currentchannel);
    return ;
  }
  Channel *channel_type = findChannel(channelname);
  if (channel_type == NULL && !channelname.empty()) //Checkes if the channel doesnt exist
  {
    this->sendNumeric(clientfd, 403, channelname, std::vector<std::string>(), "No such channel");
    return ;
  }
  if (channel_type != NULL && channel_type->getInviteonly() == false)
  {
    //Switch channels
    user->setCurrentChannel(channelname);
    this->sendNotice(clientfd, channelname, "You have switched to channel: " + channelname);
    return ;
  }
}