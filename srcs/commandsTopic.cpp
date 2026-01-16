// Commands_Topic.cpp	// TOPIC
#include "../includes/Server.hpp"


//TOPIC change the channel and either view the current channel that the user is in or display all channels
//Gonna be honest im unsure which one it actually is, looking into it i might of been wrong and it only displays the current channel topic and changes the current channel topic

void Server::topic(std::string channelname, std::string maintopic, int clientfd)
{
  //Changing Channel 
  //Note: that this should only be executed when the TOPIC command has an additional prameter 
  //Check if the channel exists 
  Client *user = findClientByNickOrUser(clientfd, ""); 
  Channel *channel_type = findChannel(channelname);
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
    {
      this->sendNumeric(clientfd, 332, channelname, std::vector<std::string>(1, channel_type->getTopic()), "Current topic for " + channelname + " is:");
      this->broadcastMessage("TOPIC", channelname, user->getNickname(), user->getUsername(), channel_type->getTopic());
    }
    return ;
  }

  if (channel_type->getTopicpriv() == false) //allows anyone is change the topic
  {
    //Should the broadcast message include the RPL_TOPIC 332 numeric reply?
    channel_type->setTopic(maintopic);
    this->broadcastMessage("TOPIC", channelname, user->getNickname(), user->getUsername(), channel_type->getTopic());
  }
  else if ((channel_type->getTopicpriv() == true && channel_type->IsOperator(clientfd) == true) || channel_type->getTopicpriv() == false)
  {
    channel_type->setTopic(maintopic);
    this->broadcastMessage("TOPIC", channelname, user->getNickname(), user->getUsername(), channel_type->getTopic());
  }
  else if (channel_type->getTopicpriv() == true && channel_type->IsOperator(clientfd) == false)
  {
    this->sendNumeric(clientfd, 482, user->getNickname(), std::vector<std::string>(1, channelname), "You are not an operator!");
  }
  return ;
}