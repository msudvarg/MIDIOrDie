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
#include "../fft2midi/channelbroker.h"

ChannelBroker channel_broker;

int port = 0;
bool drum = false;
bool all = false;
bool hillclimb = false;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}


void socket_recv(Socket::Connection * client) {

    Channel channel(channel_broker, drum);

    std::cout << "Connecting to MIDI port: " << port << " ..." << std::endl;
    Desynthesizer desynth {port, channel.get_channel(), all, hillclimb};
    shared_fft_t::array_type & fft_data = desynth.fft_data();

    //Loop and do stuff
    while(client->isrunning() && !quit) {
        
        Poller poller(WINDOW_LATENCY_MS);

        //Read from socket into Desynth object. Contents can be found in desynth.tone.interval
        client->recv(
            fft_data.data(),
            sizeof(shared_fft_t::value_type) * shared_fft_t::size);

        desynth.run();

    }

}

int main(int argc, char** argv) {

    //Register signal handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    //Get command-line options
    int opt;
  
    while((opt = getopt(argc, argv, "adp:c")) != -1) {
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
            case 'c':
              hillclimb = true;
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
