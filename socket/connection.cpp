#include <exception>
#include <iostream>

#include "connection.h"

namespace Socket {

//Constructor
Connection::Connection(
    const int cfd_,
    void (*f_)(Connection*)) :
    //Initialize member variables
    cfd {cfd_},
    running {true},
    f {f_}
{
    //Run thread
    t = std::thread { [this] {this->run();} };
}

//Move constructor
Connection::Connection(Connection && other) noexcept :
    cfd {other.cfd},
    t {std::move(other.t)},
    running {other.running},
    f {other.f}
{
    other.cfd = -1; //So that a valid file descriptor isn't closed when moved-from is destroyed
}

//Move assignment operator
Connection & Connection::operator=(Connection && other) noexcept {
    cfd = other.cfd;
    t = std::move(other.t);
    running = other.running;
    f = other.f;
    
    other.cfd = -1; //So that a valid file descriptor isn't closed when moved-from is destroyed
    
    return *this;
}

//Destructor: close socket, stop and join thread
Connection::~Connection() {
    close(cfd);
    running = false;
    if(t.joinable()) t.join();
}

//Run registered function
void Connection::run() {
    try {
        f(this);
    }
    catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception! "<< std::endl;
    }
}

//Send data over socket
void Connection::send(void * buf, size_t count) {
    size_t remaining = count;
    while (remaining > 0) {
        ssize_t ret = write(cfd, buf, count);
        if (ret == -1) {
            running = false;
            return;
        }
        remaining -= ret;
    }
}

//Receive data from socket
void Connection::recv(void * buf, size_t count) {
    size_t remaining = count;
    while (remaining > 0) {
        ssize_t ret = read(cfd, buf, count);
        if (
            ret == -1 || //Read error
            (count > 0 && ret == 0)) //End of file
        {
            running = false;
            return;
        }
        remaining -= ret;
    }
}

}
