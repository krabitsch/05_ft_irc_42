
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
#include <fcntl.h>
#include <string>
#include <map>
#include <vector>
#include "Channel.hpp"

class Server
{
  private:
  std::vector<Channel> channels; //A vector of all the channels 
  //Should we have a vector of all current clients conencted to the server

  public:
  Server();
  ~Server();
  Server(const Server &type);
	Server &operator=(const Server &type1);

  void topic(std::string channelname); //Topic Command
};



#endif