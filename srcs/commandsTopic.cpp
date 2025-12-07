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

  Channel *channel_type = findChannel(user->getCurrentChannel());
  if (channel_type == NULL) //Checkes if the channel doesnt exist
  {
    this->sendNumeric(clientfd, 403, channelname, std::vector<std::string>(), "No such channel");
    return ;
  }
  if (channel_type != NULL) //Change the topic name
  {
    if (channel_type->getTopicpriv() == true && channel_type->IsOperator(clientfd) == true || channel_type->getTopicpriv() == false)
    {
      channel_type->channelTopic(channelname);
      this->sendNotice(clientfd, channel_type->getname(), "Channel topic has been changed to: " + channelname);
    }
    else if (channel_type->getTopicpriv() == true && channel_type->IsOperator(clientfd) == false)
    {
      this->sendNumeric(clientfd, 482, "", std::vector<std::string>(), "You are not an operator!");
    }
    return ;
  }
}