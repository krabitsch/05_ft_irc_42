#include "../includes/Client.hpp"

//Orothodox For Client Class
Client::Client() {};
Client::~Client() {};
Client::Client(const Client &type) {};
Client &Client:: operator=(const Client &type1) {};

//Accepting New Client
//Accepts client in the server if they dont already exist 

//Discounting Client or use the quit command in commandsAuth

//Need a function where we get the specific client 

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

std::map<std::string, char> *Client::GetChannel(void)
{
	return (&channels);
}