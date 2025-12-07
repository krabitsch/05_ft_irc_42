// Commands_Auth.cpp	 // PASS/NICK/USER/QUIT
#include "../includes/Server.hpp"

  //Pass
void	Server::passCommand(Client &client, const IrcCommand &cmd)
{
	if (client.isRegistered())
	{
		// 462 ERR_ALREADYREGISTRED
		this->sendNumeric(client.getFd(), 462, client.getNickname(), std::vector<std::string>(),
					"You are already connected and cannot handshake again"); // libera gives this
		return ;
	}

	if (cmd.parameters.empty()) {
		// 461 ERR_NEEDMOREPARAMS
		this->sendNumeric(client.getFd(), 461, "*", std::vector<std::string>(1, "PASS"),
					"Not enough parameters");  // libera gives this
		return ;
	}

	const std::string &passwordInput = cmd.parameters[0];

	if (passwordInput != this->_password)
	{
		// 464 ERR_PASSWDMISMATCH
		this->sendNumeric(client.getFd(), 464, "*", std::vector<std::string>(),
					"Password incorrect");
		// usually disconnect
		this->clearClients(client.getFd());
		close(client.getFd());
		return ;
	}

	client.setHasPass(true);
	//client.setPassword(passwordInput);

	this->tryRegisterClient(client);
}

  //Nick
  //Sets the nickname of the user

void	Server::nickCommand(Client &client, const IrcCommand &cmd)
{
	if (cmd.parameters.empty())
	{
		this->sendNumeric(client.getFd(), 431, "*",	std::vector<std::string>(),
					"No nickname given");
		return ;
	}

	std::string newNick = cmd.parameters[0];
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getNickname() == newNick && this->_clients[i].getFd() != client.getFd())
		{
			// 433 ERR_NICKNAMEINUSE
			this->sendNumeric(client.getFd(), 433, newNick,	std::vector<std::string>(),	"Nickname is already in use");
			return ;
		}
	}

	client.setNickname(newNick);
	client.setHasNick(true);

	this->tryRegisterClient(client);

}

void Server::nickComand(int fd, std::string newname)
{
	Client *user = findClient(fd, "");
	if (findClient(-1, newname) != NULL)// Checks if the nickname has already been taken or not
	{
	  std::cout << "This nickname is already taken!" << std::endl;  // cout displays in terminal where ircserv runs
	  return ;
	}
	user->setNickname(newname); 
	std::cout << "Your new nickname is " << newname << std::endl;

}

//User
//Unsure what this does
void	Server::userCommand(Client &client, const IrcCommand &cmd)
{
	// KR: need to still implement this, for now setHasUser = true, so registration can be completed and other commands tested
	if (client.isRegistered())
	{
		// 462 ERR_ALREADYREGISTRED
		this->sendNumeric(client.getFd(), 462, client.getNickname(), std::vector<std::string>(),
					"You are already connected and cannot handshake again"); // libera gives this
		return ;
	}
	client.setHasUser(true);

	this->tryRegisterClient(client);
}

  //Quit 
  //Exits the server 

  void quit()
  {

  }