#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "portaudio.h"

#include "../include/manifest.h"
#include "../socket/socket.h" //Socket wrapper
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
#include "../fft2midi/fft2midi.h"

//If drum is specified, only first client gets the drum
std::mutex drumlock;
bool drum = false;

bool all = false;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}


void socket_recv(Socket::Connection * client) {

    bool localdrum;

    //Obtain the drum
    {
        std::lock_guard<std::mutex> lk {drumlock};
        localdrum = drum;
        if (localdrum) drum = false;
    }

    //Hacky, but should work if there aren't too many clients or other open file descriptors
    int port = client->get_cfd() - 4;

    Desynthesizer desynth {port, drum, all};
    FFT::Shared_Array_t::array_type & fft_data = desynth.fft_data;

    //Loop and do stuff
    while(client->isrunning()) {
        
        Poller poller(polling_freq);

        //Read from socket into MIDIExtraction object
        client->recv(
            fft_data.data(),
            sizeof(FFT::Shared_Array_t::value_type) * FFT::Shared_Array_t::size);

        desynth.run();

    }

    //Give back the drum on disconnect
    {
        std::lock_guard<std::mutex> lk {drumlock};
        if (localdrum) drum = true;
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
  
    while((opt = getopt(argc, argv, "ad:")) != -1) {
        switch(opt) {
            case 'a':
                all = true;
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
