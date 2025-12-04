  #include "../includes/Server.hpp"
  
  // Commands_Msg.cpp      // PRIVMSG/NOTICE

  //Privmsg 
  //Step 1:Get the username or nickname, look through it from the server, if they dont exist display a message carry on if they do
 

  void Server::privateMsg(std::string username, std::string msg)
  {
    std::cout << "Executing PRIVMSG to " << username << " with message: " << msg << std::endl;
    Client *client = findClient(-1, username);
    if (client == NULL)
    {
      this->sendNumeric(-1, 401, username, std::vector<std::string>(), "No such nick/channel");
      return ;
    }

    //send the msg to the user im not really sure how to do th at XD
    this->sendMessage(client->getFd(), "", "PRIVMSG", std::vector<std::string>(1, username), msg);

    //Old Message
		/*ssize_t sent = send(client->getFd(), msg.c_str(), msg.size(), 0);
		if (sent == -1) 
		{
		  std::cerr << "send() error on fd " << client->getFd() << ": " << std::strerror(errno) << std::endl;
		}*/
  }

  //Notice
  //Idk what that is ask someone, maybe the client check if they received any request or notifications 
  void notice()
  {
    
  }