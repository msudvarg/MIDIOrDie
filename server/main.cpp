#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <thread>
#include "portaudio.h"

#include "../include/manifest.h"
#include "../socket/socket.h" //Socket wrapper
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
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

    Desynthesizer desynth {port, drum, all);
    FFT::Shared_Array_t::array_type & fft_data = desynth.fft_data;

    //Loop and do stuff
    while(client->isrunning()) {
        
        Poller poller(polling_freq);

        //Read from socket into MIDIExtraction object
        client->recv(
            fft_data,
            sizeof(decltype(fft_data)::value_type) * decltype(fft_data)::size);

        desynth.run();

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
            //Socket connections handle everything
            std::this_thread::yield();
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
