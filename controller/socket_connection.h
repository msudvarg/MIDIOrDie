#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "scoped_thread.h"

class Socket_Connection {

    int cfd;
    scoped_thread st;
    void (*f)(Socket_Connection*);

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Connection_Error {};

    Socket_Connection(const int cfd_, void (*f_)(Socket_Connection*));
    ~Socket_Connection();
};

Socket_Connection::Socket_Connection(const int cfd_, void (*f_)(Socket_Connection*)) :
    cfd {cfd_},
    f {f_}
{

    std::thread t(f,this);
    st {t};
}

Socket_Connection::~Socket_Connection() {
    close(cfd);
}