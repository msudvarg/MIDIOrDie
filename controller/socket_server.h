#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include "socket_connection.h"
#include "socket_manifest.h"

class Socket_Server {

private: 

    //Socket information
    int sfd;
	struct sockaddr_in addr;

    //Connected clients
    const int max_clients;
    std::vector<Socket_Connection> clients; 

    std::thread t; //Active object thread   
    bool running; //Socket state    
    void (*f)(Socket_Connection*); //Registered function to interract with socket

    void accept_clients(); //Thread function to listen on socket and accept new clients

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Bind_Error {};
    class Listen_Error {};

    //Constructor
    Socket_Server(const char * ipaddr,
        const int portno,
        void (*f_)(Socket_Connection*),
        const int max_clients_ = 0);

    //Destructor
    ~Socket_Server();
};
