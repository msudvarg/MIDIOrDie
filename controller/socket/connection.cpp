#include "connection.h"

namespace Socket {

//Constructor
Connection::Connection(
    const int cfd_,
    void (*f_)(Connection*)) :
    //Initialize member variables
    cfd {cfd_},
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
{}

//Move assignment operator
Connection & Connection::operator=(Connection && other) noexcept {
    cfd = other.cfd;
    t = std::move(other.t);
    running = other.running;
    f = other.f;
    return *this;
}

//Destructor: close socket, stop and join thread
Connection::~Connection() {
    close(cfd);
    running = false;
    if(t.joinable()) t.join();
}

//Run registered function until socket disconnects
void Connection::run() {
    while (running) {
        f(this);
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

}