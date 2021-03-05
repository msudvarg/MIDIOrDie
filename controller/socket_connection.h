#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

class Socket_Connection {

private:

    //Connection information
    int cfd;

    std::thread t; //Active object thread   
    bool running; //Socket state    
    void (*f)(Socket_Connection*); //Registered function to interract with socket

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Connection_Error {};

    //Constructor
    Socket_Connection(
        const int cfd_,
        void (*f_)(Socket_Connection*));

    //Destructor
    ~Socket_Connection();
};