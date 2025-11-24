#include "../includes/Client.hpp"

//Orothodox For Client Class
Client::Client(int socket, std::string name): fd(socket), username(name) 
{
    nickname = nullptr;
    password = nullptr;
};

Client::~Client() {}; //For the decontructor im unsure whether or not we completely delete the user from every channel or we retain that info
Client::Client(const Client &type) {};
Client &Client:: operator=(const Client &type1) { return *this;};

//Setters/Getters and an insert function
void Client::addNofitication(std::string msg, char type)
{
    notifications.insert({type, msg});
}

void Client::SetChannel(std::map<std::string, char> *newchannels)
{
    channels = *newchannels;
}

void Client::AddChannel(std::string channelname)
{
    channels.insert({channelname, 'm'});    
}
void Client::SetNickname(std::string newname)
{
    nickname = newname;
}
std::string Client::GetNickname(void)
{
    return (nickname);
}
int Client::GetFdSocket(void)
{
	return (fd);
}

std::string Client::GetUsername(void)
{
    return (username);
}

std::string Client::GetCurrentChannel(void)
{
    return (current_channel);
}

void Client::SetCurrentChannel(std::string newchannel)
{
    current_channel = newchannel;
}

std::map<std::string, char> *Client::GetChannel(void)
{
	return (&channels);
}