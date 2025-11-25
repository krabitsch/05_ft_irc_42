/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:56:54 by krabitsc          #+#    #+#             */
/*   Updated: 2025/11/25 12:07:04 by aruckenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cerrno>
#include <climits>


int	checkInputArgs(int ac, char **av, int* port, int* password)
{
	if (ac != 3)
	{
		std::cout << "Wrong number of input arguments.\n"
				  << "Expected format:\n"
				  << "./ircserv <port> <password>" << std::endl;
		return (-1);
	}

   long	values[2];
	for (int i = 1; i <= 2; ++i)
	{
		std::string arg(av[i]);

		for (size_t j = 0; j < arg.size(); ++j)
		{
			if (j == 0 && arg[j] == '+')
				continue ;
			if (!std::isdigit(static_cast<unsigned char>(arg[j])))
			{
				std::cerr << "Error: Arguments can only be positive integers." << std::endl;
				return (-1);
			}
		}
		char* endptr = 0;
		errno = 0;
		long num = std::strtol(av[i], &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || num < 0 || num > INT_MAX)
		{
			std::cerr << "Error: Arguments must be valid positive integers within range." << std::endl;
			return (-1);
		}
		values[i - 1] = num;
	}

	int portVal	 = static_cast<int>(values[0]);
	int passwordVal = static_cast<int>(values[1]);

	// check port ranges:
	if (portVal <= 0 || portVal > 65535)
	{
		std::cerr << "Error: Port must be in range 1–65535." << std::endl;
		return (-1);
	}
	else if (portVal <= 1023)
	{
		std::cerr << "Warning: Ports 1–1023 are reserved (root-only). " << "Pick a port >= 1024, e.g. 4444." << std::endl;
		return (-1);
	}

	*port	 = portVal;
	*password = passwordVal;

	return (0);
}

int main(int ac, char **av)
{
	int		port;
	int		password;

	if (checkInputArgs(ac, av, &port, &password) != 0)
		return (-1);

	Server	server(port, password); // call constructor with port = av[1], password = av[2]

	server.setServerAdd(&server); //Sets the server class inside to itself
	
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