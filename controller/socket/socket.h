#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "connection.h"
#include "socket_manifest.h"

namespace Socket {
    
class Socket {

protected:

    //Socket information
    int sfd;
	struct sockaddr_in addr;

    //Registered function to interract with socket
    void (*f)(Connection*);

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};

    //Constructor
    Socket(const char * ipaddr,
        const int portno,
        void (*f_)(Connection*));

    //Destructor
    ~Socket() = default;
};


}
