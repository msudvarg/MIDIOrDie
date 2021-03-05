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

    void run(); //Thread function to continually run registered function

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Connection_Error {};

    //Constructor
    Socket_Connection(
        const int cfd_,
        void (*f_)(Socket_Connection*));

    //Copy construction and assignment not supported
	Socket_Connection(Socket_Connection const &) = delete;
	Socket_Connection & operator=(Socket_Connection const &) = delete;

    //Move construction and assignment
	Socket_Connection(Socket_Connection && other) noexcept;
	Socket_Connection & operator=(Socket_Connection && other) noexcept;

    //Destructor
    ~Socket_Connection();
};