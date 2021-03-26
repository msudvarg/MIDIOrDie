#include <signal.h>
#include <getopt.h>
#include <iostream>
#include "portaudio.h"

#include "socket/socket.h" //Socket wrapper
#include "socket_manifest.h" //Functions to pass to socket connections
#include "shared_array.h" //Thread-safe array
#include "fft.h"
#include "poller.h"

using namespace std;

FFT fft;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
void sigint_handler(int signum) {
    fft.end();
}


void socket_send(Socket::Connection * client) {
    
    //TODO: What's our send rate?
    Poller poller(50); //50 ms?

    //Copy shared array to local array
    FFT::Shared_Array_t::array_type localArray = fft.read();

    //Send local array over socket
    client->send(
        localArray.data(),
        sizeof(FFT::Shared_Array_t::value_type) * FFT::Shared_Array_t::size);

}

int main(int argc, char** argv) {

    //Register signal handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);



    int opt;
    bool forever = false;
    string ipaddr = "127.0.0.1";

    //Get command-line options
    while((opt = getopt(argc, argv, "fi:")) != -1) {
        switch(opt) {

        //Run forever
        case 'f':
            forever = true;
            break;

        //IP Address
        case 'i':
            if(optarg) ipaddr.assign(optarg);
            break;

        default:
            std::cerr << "Usage: " << argv[0] << " [-f] [-i0.0.0.0]\n" << std::endl;
        }
    }

    //Exception handling

    try {

        fft.init();

        //Create socket to send data
        //Keep looping on connection error in case server has not been set up
        while(true) {
            try {
                Socket::Client socket {ipaddr.c_str(), PORTNO, socket_send};
            }
            catch (Socket::Connection_Error &) { continue; }
            break;
        }

        fft.run(forever);

    }
    catch (PaError ret) {
        return ret;
    }
    catch (int ret) {
        return ret;
    }
    catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception! "<< std::endl;
        return -1;
    }
    
    //Create shared memory for visualization
    //Shared_Memory<Shared_Buffer> sharedBuffer {"fftData"};



    return 0;    
}
