/******************************************************************************
 bare bones TCP server, based on youtube video:
 "Creating a TCP server in C++ [Linux/ Code Blocks]", by Sloan Kelly
 https://www.youtube.com/watch?v=cNdlrbZSkyQ&list=PLHBVNH27RbWqGTL-AYMylWkNck45cxPnG&index=1

 Instructions to run: compile as: 
 c++ bbtcpserver.cpp -o server
 ./server

 in a separate terminal: 
 telnet localhost 54000 

*******************************************************************************/

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int main()
{
    // Create a socket
    int fdListening = socket(AF_INET, SOCK_STREAM, 0);
    if (fdListening == -1)
    {
        std::cerr << "Can't create a socket!" << std::endl;
        return (-1);
    }
    
    
    // Bind the socket to an IP/export
    sockaddr_in hint;             // maybe a better name for hint would be serverAddress
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); // takes host shorts and converts them to network shorts
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); // want to run it on/ bind it to any IP address
                                                   // for it to take any address: 0.0.0.0
                                                   // number to an array of , e.g. 127.0.0.1
	if (bind(fdListening, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return (-2);
    }
    
    // Mark the socket for listening in
    if (listen(fdListening, SOMAXCONN) == -1) // SOMAXCONN: how many connections it can listen to
    {
        std::cerr << "Can't listen" << std::endl;
        return (-3);
    }
    
    // Accept a call
    sockaddr_in client;
    socklen_t   clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    int clientSocket = accept(fdListening, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1)
    {
        std::cerr << "Problem with client connecting!" << std::endl;
        return (-4);
    }
    close (fdListening);
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);
    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (result == 0)
    {
        std::cout << host << " connected on " << svc << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }
    
    // While receiving, display message, echo message
    char buf[4096];
    while (true)
    {
        // clear the buffer
        memset(buf, 0, 4096);
        // wait for a message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1)
        {
            std::cerr << "There was a connection issue" << std::endl;
            break ;
        }
        if (bytesRecv == 0)
        {
            std::cerr << "The client disconnected" << std::endl;
            break ;
        }
        // Display message
        std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;
        // Resend message
        send(clientSocket, buf, bytesRecv, 0);
    }
    
    
    // Close the listening socket
    close(clientSocket);

    return (0);
}