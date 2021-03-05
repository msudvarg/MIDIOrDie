#include "socket_connection.h"

//Constructor
Socket_Connection::Socket_Connection(const int cfd_, void (*f_)(Socket_Connection*)) :
    cfd {cfd_},
    f {f_}
{

    std::thread t(f,this);
}

Socket_Connection::~Socket_Connection() {
    close(cfd);
}