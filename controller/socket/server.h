#pragma once

#include <vector>
#include "base.h"
#include "connection.h"

namespace Socket {

class Server: public Socket {

private: 

    //Connected clients
    const int max_clients;
    std::vector<Connection> clients; 

    std::thread t; //Active object thread   
    bool running; //Socket state    

    //Thread function to listen on socket and accept new clients
    void accept_clients();

public:

    //Constructor
    Server(const char * ipaddr,
        const int portno,
        void (*f_)(Connection*),
        const int max_clients_ = 0);

    //Destructor
    ~Server();
};

}