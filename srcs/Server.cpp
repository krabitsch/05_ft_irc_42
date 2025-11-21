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

Client* Server::findClient(const int fd)
{
  std::map<int, Client>::iterator it = clients.find(fd);
  if (it == clients.end())
    return NULL;

    return (&it->second);
}