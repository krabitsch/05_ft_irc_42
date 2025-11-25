  #include "../includes/Server.hpp"
  
  // Commands_Msg.cpp      // PRIVMSG/NOTICE

  //Privmsg 
  //Step 1:Get the username or nickname, look through it from the server, if they dont exist display a message carry on if they do
 
  //The Question is now for step 2, do you send a direct msg to the user or open a private channel between the two of you
  //Step 2:Here you would just send it directly to the user 

  void Server::privateMsg(std::string username, std::string msg)
  {
    Client *client = findClient(-1, username);
    if (client == NULL)
    {
      std::cerr << "User " << username << " does not exist in this server!" << std::endl;
      return ;
    }
    //send the msg to the user im not really sure how to do th at XD

  }

  //Notice
  //Idk what that is ask someone, maybe the client check if they received any request or notifications 
  void notice()
  {
    
  }