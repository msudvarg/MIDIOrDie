#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>

namespace Socket {

//Manifest constants
constexpr int default_backlog = 10;

//Exceptions
struct No_Socket : std::exception {
    const char* what() const noexcept {return "Could not open socket!\n";}
};

struct Invalid_IP : std::exception {
    const char* what() const noexcept {return "Invalid IP address!\n";}
};

struct Bind_Error : std::exception { 
    const char* what() const noexcept {return "Could not bind to socket!\n";}
};

struct Listen_Error : std::exception { 
    const char* what() const noexcept {return "Could not listen to socket!\n";}
};

struct Connection_Error : std::exception { 
    const char* what() const noexcept {return "Could not connect to socket!\n";}
};

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
