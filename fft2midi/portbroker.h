#pragma once

#include <set>
#include <exception>
#include <mutex>


class PortBroker {
    
private:
    unsigned int port_count;
    int drum_port = -1;
    std::set<unsigned int> ports;
    std::mutex m;

public:

    bool getPort(unsigned int & port, bool & drum);
    void freePort (unsigned int port);

    PortBroker();
};

class Port {

private:
    PortBroker & broker;
    unsigned int port;
    bool drum;

public:

    struct No_Port : std::exception {
        const char* what() const noexcept {return "No available ports!\n";}
    };    

    unsigned int get_port() const { return port; }
    bool get_drum() const { return drum; }

    Port(PortBroker & _broker, bool _drum);

    ~Port() {
        broker.freePort(port);
    }

};