  // Commands_Topic.cpp    // TOPIC

//TOPIC change the channel and either view the current channel that the user is in or display all channels
//Gonna be honest im unsure which one it actually is

#include "../includes/Server.hpp"

void Server::topic(std::string channelname)
{

  //Changing Channel 
  //Note: that this should only be executed when the TOPIC command has an additional prameter 
  //Check if the channel exists 
  int i = 0;
  while (channels[i].name)
  {
    if (channels[i].name == channelname)
    {
      //When the Client exectues a msg or command there client socket is also saved so using that socket id we can identify the user 
      // and there information in order to transfer them to a new channel
      //add user to channel
      //deleteuser from channel
    }
    i++;
  }

  //Display current Channel or Avaiable channels im unsure about this 

}