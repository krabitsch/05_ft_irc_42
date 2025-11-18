/*!
 
 * KR: made a couple of modifications to the original server.cpp and client.cpp found at the github side below
 * in particular, some of the variable names given in the original server.cpp file were a bit confusing
 * youtube video can be found here: https://www.youtube.com/watch?v=IydkqseK6oQ&list=PLHBVNH27RbWqGTL-AYMylWkNck45cxPnG&index=9

 * Simple chat program (server side).cpp - http://github.com/hassanyf
 * Version - 2.0.1
 *
 * Copyright (c) 2016 Hassan M. Yousuf
 */

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

//using namespace std;

int main()
{
    /* ---------- INITIALIZING VARIABLES ---------- */

    /*  
       1. fdServerSocketListening/ fdClientSocket are two file descriptors
       These two variables store the values returned 
       by the socket system call and the accept system call.

       2. portNum is for storing port number on which
       the accepts connections

       3. isExit is a bool variable which will be used to 
       end the loop

       4. The server reads characters from the socket 
       connection into a dynamic variable (buffer).

       5. A sockaddr_in is a structure containing an internet 
       address. This structure is already defined in netinet/in.h, so
       we don't need to declare it again.

        DEFINITION:

        struct sockaddr_in
        {
          short   sin_family;
          u_short sin_port;
          struct  in_addr sin_addr;
          char    sin_zero[8];
        };

        6. serv_addr will contain the address of the server

        7. socklen_t  is an intr type of width of at least 32 bits


    */
    int fdServerSocketListening, fdClientSocket;
    int portNum = 1500;
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];

    struct sockaddr_in	server_addr;
    socklen_t 			size;

    /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    /* --------------- socket() function ------------------*/

    fdServerSocketListening = socket(AF_INET, SOCK_STREAM, 0);
	// KR comments:
	// 'fdServerSocketListening' was, confusingly, called 'client' in original github code.
	// renamed, to make more sense
	// 0 uses default protocol for this type of socket
	// equivalent to:
	// fdServerSocketListening = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	// "fdServerSocketListening" is the fd of a socket that eventually will be listening for clients (fdListening in video1)


    if (fdServerSocketListening < 0) 
    {
        std::cout << "\nError establishing socket..." << std::endl;
        return (-1);
    }

    /*
        The socket() function creates a new socket.
        It takes 3 arguments,

            a. AF_INET: address domain of the socket.
            b. SOCK_STREAM: Type of socket. a stream socket in 
            which characters are read in a continuous stream (TCP)
            c. Third is a protocol argument: should always be 0. The 
            OS will choose the most appropiate protocol.

            This will return a small integer (a file descriptor) and is used for all 
            references to this socket. If the socket call fails, 
            it returns -1.

    */

    std::cout << "\n=> Socket server has been created..." << std::endl;

    /* 
        The variable serv_addr is a structure of sockaddr_in. 
        sin_family contains a code for the address family. 
        It should always be set to AF_INET.

        INADDR_ANY contains the IP address of the host. For 
        server code, this will always be the IP address of 
        the machine on which the server is running.

        htons() converts the port number from host byte order 
        to a port number in network byte order.

    */

    server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
	server_addr.sin_port = htons(portNum);

	// KR comments: this line of code below, that was in the original code, seems to be wrong: 
    // server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
	/*
	Why it's wrong: // Mixing 32-bit and 16-bit byte order
	sin_addr.s_addr is a 32-bit field (uint32_t).
	INADDR_ANY is a 32-bit constant (value 0), meaning “bind to all local interfaces” → IP 0.0.0.0.
	htons() is host-to-network for a 16-bit value (short).
	So this line is doing:
	Take a 32-bit value (INADDR_ANY = 0)
	Pass it to a function meant for 16-bit values
	Store the result into a 32-bit IP field
	That’s simply the wrong function for that field. It “works” only because INADDR_ANY is 0, and htons(0) is 0, but it’s conceptually wrong and would definitely be wrong for any other address.
	*/
	// KR: instead, can use any of the three alternatives below:
	//server_addr.sin_addr.s_addr = INADDR_ANY;
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);

	//server_addr.sin_port = htons(portNum);
	// KR comments:
	/* 
	Why sin_port must use htons()
		sin_port is a 16-bit field (type uint16_t), because TCP/UDP ports range from 0–65535.
		htons() stands for: host to network short (16-bit)
	does exactly what you want:
		converts a 16-bit number (your port)
		from host byte order (little endian on x86)
		to network byte order (big endian on the wire)
	htons()   // for 16-bit values (ports)
	htonl()   // for 32-bit values (IPv4 addresses)
	*/


    /* ---------- BINDING THE SOCKET ---------- */
    /* ---------------- bind() ---------------- */


    if ((bind(fdServerSocketListening, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) 
    {
        std::cout << "=> Error binding connection, the socket has already been established..." << std::endl;
        return (-2);
    }

    /* 
        The bind() system call binds a socket to an address, 
        in this case the address of the current host and port number 
        on which the server will run. It takes three arguments, 
        the socket file descriptor. The second argument is a pointer 
        to a structure of type sockaddr, this must be cast to
        the correct type.

		comment KR: sockaddr* is the pointer to the generic sockaddr (could be sockaddr_in, sockaddr_in6, ...)
    */

    size = sizeof(server_addr);
    std::cout << "=> Looking for clients..." << std::endl;

    /* ------------- LISTENING CALL ------------- */
    /* ---------------- listen() ---------------- */

   if (listen(fdServerSocketListening, 1) == -1)
    {
        std::cerr << "Can't listen" << std::endl;
        return (-3);
    }	
    //listen(fdServerSocketListening, 1);

    /* 
        The listen system call allows the process to listen 
        on the socket for connections. 

        The program will stay idle here if there are no 
        incoming connections.

        The first argument is the socket file descriptor, 
        and the second is the size for the number of clients 
        i.e the number of connections that the server can 
        handle while the process is handling a particular 
        connection. 

    */

    /* ------------- ACCEPTING CLIENTS  ------------- */
    /* ----------------- listen() ------------------- */

    /* 
        The accept() system call causes the process to block 
        until a client connects to the server. Thus, it wakes 
        up the process when a connection from a client has been 
        successfully established. It returns a new file descriptor, 
        and all communication on this connection should be done 
        using the new file descriptor. The second argument is a 
        reference pointer to the address of the server on the other 
        end of the connection, and the third argument is the size 
        of this structure.
    */

    int clientCount = 1;
    fdClientSocket = accept(fdServerSocketListening,(struct sockaddr *)&server_addr,&size);
	std::cout << fdClientSocket << std::endl;

    // first check if it is valid or not
    if (fdClientSocket < 0) 
        std::cout << "Error on accepting..." << std::endl;

    while (fdClientSocket > 0) 
    {
        strcpy(buffer, "Server connected...\n");
        send(fdClientSocket, buffer, bufsize, 0);
        std::cout << "Connected with the client #" << clientCount << ", you are good to go..." << std::endl;
        std::cout << "\nEnter # to end the connection\n" << std::endl;

        /* 
            Note that we would only get to this point after a 
            client has successfully connected to our server. 
            This reads from the socket. Note that the read() 
            will block until there is something for it to read 
            in the socket, i.e. after the client has executed a 
            the send().

            It will read either the total number of characters 
            in the socket or 1024
        */

        std::cout << "Client: ";
        do
		{
            recv(fdClientSocket, buffer, bufsize, 0);
            std::cout << buffer << " ";
            if (*buffer == '#') {
                *buffer = '*';
                isExit = true;
            }
        }
		while (*buffer != '*');

        do
		{
            std::cout << "\nServer: ";
            do
			{
                std::cin >> buffer;
                send(fdClientSocket, buffer, bufsize, 0);
                if (*buffer == '#') {
                    send(fdClientSocket, buffer, bufsize, 0);
                    *buffer = '*';
                    isExit = true;
                }
            }
			while (*buffer != '*');

            std::cout << "Client: ";
            do
			{
                recv(fdClientSocket, buffer, bufsize, 0);
                std::cout << buffer << " ";
                if (*buffer == '#') {
                    *buffer = '*';
                    isExit = true;
                }
            }
			while (*buffer != '*');
        }
		while (!isExit);

        /* 
            Once a connection has been established, both ends 
            can both read and write to the connection. Naturally, 
            everything written by the client will be read by the 
            server, and everything written by the server will be 
            read by the client.
        */

        /* ---------------- CLOSE CALL ------------- */
        /* ----------------- close() --------------- */

        /* 
            Once the server presses # to end the connection,
            the loop will break and it will close the server 
            socket connection and the client connection.
        */

        // inet_ntoa converts packet data to IP, which was taken from client
        std::cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
        close(fdClientSocket);
        std::cout << "\nGoodbye..." << std::endl;
        isExit = false;
        return (-4);
    }

    close(fdServerSocketListening);
    return (0);
}