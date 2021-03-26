#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <memory>
#include "portaudio.h"

#include "../include/manifest.h"
#include "../socket/socket.h" //Socket wrapper
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
#include "../controller/fft.h"
#include "../fft2midi/fft2midi.h"

int port = 0;
bool drum = false;
bool all = false;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}


void socket_recv(Socket::Connection * client) {

    //TODO: Declare and construct MIDIExtraction object
    Desynthesizer desynth {port, drum, all);

    //Loop and do stuff
    while(client->isrunning()) {
        
        Poller poller(polling_freq);

        //TODO: Read from socket into MIDIExtraction object
        /*
        client->recv(
            midi.data(),
            sizeof(decltype(sharedArray)::value_type) * decltype(sharedArray)::size); */

    }

}

int main(int argc, char** argv) {

    //Register signal handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    //Get command-line options
    //TODO: Add command-line options for MIDIExtraction
    int opt;
  
    while((opt = getopt(argc, argv, "adp:")) != -1) {
        switch(opt) {
            case 'a':
                all = true;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
                drum = true;
                break;
        }
    }

    //Exception handling

    try {
        
        //Socket server to receive FFT data from client
        static Socket::Server socket_server {IPADDR, PORTNO, socket_recv};

        while(!quit) {
            Poller poller (polling_freq);
        }

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

    return 0;    
}
