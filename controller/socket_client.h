#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "scoped_thread.h"

class Socket_Client {

private:

    //Socket information
    int sfd;
	struct sockaddr_in addr;

    std::thread t; //Active object thread   
    bool running; //Socket state    
    void (*f)(Socket_Client*); //Registered function to interract with socket

    void run(); //Thread function to continually run registered function

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Connection_Error {};

    //Send Data
    void send(void * buf, size_t count);

    //Constructor
    Socket_Client(
        const char * ipaddr,
        const int portno,
        void (*f_)(Socket_Client*));

    //Destructor
    ~Socket_Client();
};