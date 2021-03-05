#include "socket_connection.h"

//Constructor
Socket_Connection::Socket_Connection(
    const int cfd_,
    void (*f_)(Socket_Connection*)) :
    //Initialize member variables
    cfd {cfd_},
    f {f_}
{
    //Run thread
    t = std::thread { [this] {this->run();} };
}

//Destructor: close socket, stop and join thread
Socket_Connection::~Socket_Connection() {
    close(cfd);
    running = false;
    if(t.joinable()) t.join();
}

//Run registered function until socket disconnects
void Socket_Connection::run() {
    while (running) {
        f(this);
    }
}