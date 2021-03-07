#pragma once

#include <memory>
#include "socket.h"

namespace Socket {

class Client: public Socket {

private:

    std::unique_ptr<Connection> connection;

public:


    //Constructor
    Client(
        const char * ipaddr,
        const int portno,
        void (*f_)(Connection*));

    //Destructor
    ~Client();
};

}