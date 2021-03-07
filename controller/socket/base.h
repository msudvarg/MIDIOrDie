#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace Socket {

//Manifest constants
constexpr int default_backlog = 10;

//Exceptions
class No_Socket {};
class Invalid_IP {};
class Bind_Error {};
class Listen_Error {};
class Connection_Error {};

//Forward declaration
class Connection;
    
class Socket {

protected:

    //Socket information
    int sfd;
	struct sockaddr_in addr;

    //Registered function to interract with socket
    void (*f)(Connection*);

public:

    //Constructor
    Socket(const char * ipaddr,
        const int portno,
        void (*f_)(Connection*));

    //Pure virtual destructor to make abstract
    virtual ~Socket() = 0;
};

//Default pure virtual destructor implementation
inline Socket::~Socket() {}

}
