#include "../includes/Server.hpp"

Channel* Server::findChannel(const std::string &name)
{
  //This function looks for the specific channel
  size_t i = 0;
  while (i < channels.size())
  {
      if (channels[i].getname() == name)
        return (&channels[i]);
    i++;
  }
  return NULL;

}

//maybe make this template

Client* Server::findClient(const int fd, std::string username) //THis doesnt really work maybe change this into a template
{
  if (fd != -1)
  {
    std::map<int, Client>::iterator it = clients.find(fd);
    if (it != clients.end())
      return (&it->second);
  }

  if (!username.empty())
  {
    std::map<int, Client>::iterator it = clients.begin();
    while (it != clients.end())
    {
      if (it->second.GetUsername() == username || it->second.GetNickname() == username)
        return (&it->second);
      it++;
    }
  } 

  return (NULL);
}