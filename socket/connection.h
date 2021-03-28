#pragma once

#include <thread>
#include "base.h"

namespace Socket {

class Connection {

private:

    //Connection information
    int cfd;

    std::thread t; //Active object thread   
    bool running; //Socket state    
    void (*f)(Connection*); //Registered function to interract with socket

    void run(); //Thread function to continually run registered function

public:
    
    //Send and receive over socket
    void send(void * buf, size_t count);
    void recv(void * buf, size_t count);

    //Check running status
    bool isrunning() const { return running; }

    //Get cfd
    int get_cfd() const { return cfd; }

    //Constructor
    Connection(
        const int cfd_,
        void (*f_)(Connection*));

    //Copy construction and assignment not supported
	Connection(Connection const &) = delete;
	Connection & operator=(Connection const &) = delete;

    //Move construction and assignment
	Connection(Connection && other) noexcept;
	Connection & operator=(Connection && other) noexcept;

    //Destructor
    ~Connection();
};

}