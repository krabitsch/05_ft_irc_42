/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:56:54 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/30 13:19:56 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Instructions:
// start server in a terminal window:
// ./ircserv 4444 1234
// start client in separate terminal window, running:
// nc -C localhost 4444
// -C flag ensures nc sends \r\n (not just \n)

#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"
#include <cerrno>
#include <climits>


int	checkInputArgs(int ac, char **av, int* port, std::string* password)
{
	if (ac != 3)
	{
		std::cout << "Wrong number of input arguments.\n"
				  << "Expected format:\n"
				  << "./ircserv <port> <password>" << std::endl;
		return (-1);
	}

	// validation checks for argv[1] (i.e. argument for port; must be a positive integer)
	std::string portStr(av[1]);

	for (size_t j = 0; j < portStr.size(); ++j)
	{
		if (j == 0 && portStr[j] == '+')
			continue;
		if (!std::isdigit(static_cast<unsigned char>(portStr[j])))
		{
			std::cerr << "Error: Port argument must be a positive integer." << std::endl;
			return (-1);
		}
	}

	char *endptr = 0;
	errno = 0;
	long portNum = std::strtol(av[1], &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE || portNum <= 0 || portNum > 65535)
	{
		std::cerr << "Error: Port must be in range 1–65535." << std::endl;
		return (-1);
	}

	int portVal = static_cast<int>(portNum);

	// check privileged ports
	if (portVal <= 1023)
	{
		std::cerr << "Warning: Ports 1–1023 are reserved (root-only). "
		          << "Pick a port >= 1024, e.g. 4444." << std::endl;
		return (-1);
	}

	// password (argv[2]) can be any non-empty string 
	std::string pwdStr(av[2]);
	// password constraints
	if (pwdStr.length() == 0 || pwdStr.length() > 50)
	{
	    std::cerr << "Error: Password must be 1–50 characters long." << std::endl;
    	return (-1);
	}

	*port     = portVal;
	*password = pwdStr;

	return (0);
}

int main(int ac, char **av)
{
	int			port;
	std::string	password;

	if (checkInputArgs(ac, av, &port, &password) != 0)
		return (-1);

	Server	server(port, password); // call constructor with port = av[1], password = av[2]

	// server.setServerAdd(&server); //Sets the server class inside to itself
	
	std::cout << "---- SERVER ----" << std::endl;
	try
	{
		signal(SIGINT,  Server::signalHandler); //-> catch signal (ctrl + c)
		signal(SIGQUIT, Server::signalHandler); //-> catch signal (ctrl + \)
		server.serverInit();
	}
	catch(const std::exception& e)
	{
		server.closeFds();
		std::cerr << e.what() << std::endl;
	}
	std::cout << "The Server Closed!" << std::endl;
	return (0);
}