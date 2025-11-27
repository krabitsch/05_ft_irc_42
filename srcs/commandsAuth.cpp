  // Commands_Auth.cpp     // PASS/NICK/USER/QUIT
  #include "../includes/Server.hpp"

  //Pass
  //Idk if this is to set a password display password
  //Ask whether the pass is for the server itself or for the user
  //I think we can remove this command as we do not require a password for the user just in the begining when were verifying if they entered the correct password in the begining when trying to join the server

  //Nick
  //Sets the nickname of the user

  void Server::nickComand(int fd, std::string newname)
  {
    Client *user = findClient(fd, NULL);
    if (findClient(-1, newname) != NULL)// Checks if the nickname has already been taken or not
    {
      std::cout << "This nickname is already taken!" << std::endl;
      return ;
    }
    user->setNickname(newname); 
    std::cout << "Your new nickname is " << newname << std::endl;
  }

  //User
  //Unsure what this does

  //Quit 
  //Exits the server 

  void quit()
  {

  }