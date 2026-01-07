// Commands_Topic.cpp    // TOPIC
#include "../includes/Server.hpp"


//TOPIC change the channel and either view the current channel that the user is in or display all channels
//Gonna be honest im unsure which one it actually is, looking into it i might of been wrong and it only displays the current channel topic and changes the current channel topic

void Server::topic(std::string channelname, std::string maintopic, int clientfd)
{
  //Changing Channel 
  //Note: that this should only be executed when the TOPIC command has an additional prameter 
  //Check if the channel exists 
  Client *user = findClient(clientfd, ""); 
  Channel *channel_type = findChannel(user->getCurrentChannel());
  if (channel_type == NULL) //Checkes if the channel doesnt exist
  {
    this->sendNumeric(clientfd, 403, channelname, std::vector<std::string>(), "No such channel");
    return ;
  }

  if (maintopic.empty())
  {
    if (channel_type->getTopic().empty()) //Error msg if the channel topic is already empty
      this->sendNumeric(clientfd, 331, "", std::vector<std::string>(), "No topic is set for this channel");
    else
      this->sendNotice(clientfd, channel_type->getname(), "Current channel topic is: " + channel_type->getTopic());
    return ;
  }

  if (channel_type->getTopicpriv() == false) //allows anyone is change the topic
  {
    channel_type->setTopic(maintopic);
    this->sendNotice(clientfd, channel_type->getname(), "Channel topic has been changed to: " + maintopic);
  }
  else if (channel_type->getTopicpriv() == true && channel_type->IsOperator(clientfd) == true || channel_type->getTopicpriv() == false)
  {
    channel_type->setTopic(maintopic);
    this->sendNotice(clientfd, channel_type->getname(), "Channel topic has been changed to: " + maintopic);
  }
  else if (channel_type->getTopicpriv() == true && channel_type->IsOperator(clientfd) == false)
  {
    this->sendNumeric(clientfd, 482, "", std::vector<std::string>(), "You are not an operator!");
  }
  return ;
}