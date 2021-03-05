#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "scoped_thread.h"

class Socket_Connection {

    int sfd;
	struct sockaddr_in addr;
    scoped_thread t;

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Connection_Error {};

    Socket_Connection(const char * ipaddr, const int portno);
    ~Socket_Connection();
};

