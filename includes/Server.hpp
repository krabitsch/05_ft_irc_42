
#ifndef PMERGEME_hpp
# define PMERGEME_hpp

# ifdef FT_IRC_DEBUG
  #  define DBG(someCode) do { someCode; } while (0)
# else
  #  define DBG(someCode) do { } while (0)
# endif

# include <iostream>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstring>
# include <fcntl.h>
# include <string>
# include <map>
# include <vector>
# include "Channel.hpp"

class Server
{
  private:
  std::vector<Channel> channels; //A vector of all the channels 
  std::map<int, Client> clients;//Should we have a vector of all current clients conencted to the server

  public:
  Server();
  ~Server();
  Server(const Server &type);
	Server &operator=(const Server &type1);

  void topic(std::string channelname, int clientfd); //Topic Command

  //Find Functions
  Channel* findChannel(const std::string &name);
  Client* findClient(const int fd, std::string username);

  //Commands that need the use of the server
  void nickComand(int fd, std::string newname); //Sets the new nickanem maybe change but we will see
  void join(int fd, std::string channelname); //Creates or joins a channel that exists
  void part(int fd);
  void privateMsg(std::string username, std::string msg);


};



#endif