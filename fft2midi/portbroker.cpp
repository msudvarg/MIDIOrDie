
#include <iostream>
#include "../rtmidi/RtMidi.h"
#include "portbroker.h"

bool PortBroker::getPort(unsigned int & port, bool & drum) {

    //Lock broker
    std::lock_guard<std::mutex> lk {m};

    //Get next free port
    unsigned int free_port = 0;
    for(unsigned int consumed_port : ports) {
        if(consumed_port > free_port) break;
        else free_port++;
    }

    //If no free ports available:
    if (free_port >= port_count) return false;

    //Otherwise, assign free port:
    port = free_port;
    ports.insert(free_port);

    //Try to obtain drum
    if (drum) {
        //Drum available
        if (drum_port == -1) drum_port = port;

        //Drum unavailable
        else drum = false;
    }

    return true;

}

void PortBroker::freePort (unsigned int port) {

    //Lock broker
    std::lock_guard<std::mutex> lk {m};

    //Remove port
    ports.erase(ports.find(port));

    //Free drum if necessary
    if(drum_port == static_cast<int>(port)) drum_port = -1;
}

PortBroker::PortBroker() {
    RtMidiOut midiout;
    port_count = midiout.getPortCount();
    for (unsigned int i = 0; i < port_count; i++) {
        std::cout << i << ": " << midiout.getPortName(i) << std::endl;
    }
}

Port::Port(PortBroker & _broker, bool _drum) :
    broker (_broker),
    drum (_drum)
{
    if (!broker.getPort(port, drum)) throw No_Port{};
}