// Commands_Channel.cpp  // JOIN/PART/NAMES
#include "../includes/Server.hpp"

//Join
//Switching Channels and joining the one in the prameter 

void Server::join(int fd, std::string channelname, std::string pass)
{
	//Channel *_channel = findChannel(channelname);
    Channel* channel = findChannel(channelname);
    Client* client   = findClientByFd(fd);

    if (!client)
    {
        this->sendNumeric(fd, 401, "", std::vector<std::string>(), "nick does not exist");
        return ;
    }

    if (channel == NULL)
    {
        Channel* newChannel = new Channel(this, fd, channelname);
        this->_channels.push_back(newChannel);
        channel = newChannel;

        std::string joinMsg = ":" + client->getNickname() + " JOIN " + channelname + "\r\n";
        broadcastToChannel(channelname, joinMsg, -1); // broadcast to all channel mmembers (including self)

        return ;
    }

    ///Error Handling
    bool alreadyMember = channel->isMember(client);

    if (channel->getInviteonly() == true && alreadyMember == false)
    {
        //ERR_INVITEONLYCHAN 473
        this->sendNumeric(fd, 473, "", std::vector<std::string>(), "Cannot join channel (+i)"); //You need an invite to join
        return ;
    }

    if (channel->getUserlimit() != 0 && channel->getMembersize() >= channel->getUserlimit())
    {
        //ERR_CHANNELISFULL 471
        this->sendNumeric(fd, 471, "", std::vector<std::string>(), "Cannot join channel (+l)"); //Channel is full
        return ;
    }

    if (!channel->getPassword().empty() && channel->getPassword() != pass)
    {
        //475 ERR_BADCHANNELKEY, while 476 was ERR_BADCHANMASK
        this->sendNumeric(fd, 475, "", std::vector<std::string>(), "Cannot join channel (+k)"); //Wrong password when joining 
        return ;
    }

    // add member if not already
    if (!alreadyMember)
        channel->AddMember(client);

    client->setCurrentChannel(channelname);

    //this->sendNotice(fd, channelname, _client->getNickname() + " has joined the channel");
    // who joins is broadcasted to members of channel (including self)
    std::string joinMsg = ":" + client->getNickname() + " JOIN " + channelname + "\r\n";
    broadcastToChannel(channelname, joinMsg, -1);
}

//Part //We dont technically need this! decide whether or not we want to implement this feature!
//leaves the channel you are currently on!
void Server::part(int fd)
{
	(void)fd;
}
