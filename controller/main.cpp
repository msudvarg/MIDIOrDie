#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <memory>

#include "../include/manifest.h"
#include "../socket/socket.h" //Socket wrapper
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
#include "../localcontroller/localcontroller.h"

LocalController lc;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}


void socket_send(Socket::Connection * client) {

    shared_fft_t::array_type localArray;

    //Wait for server to be ready
    char ready;
    client->recv(&ready, 1);

    while(client->isrunning() && !quit) {
        
        Poller poller(WINDOW_LATENCY_MS);

        //Copy shared array to local array
        // shared_fft_t::array_type localArray = fft.read();

        lc.GetData(localArray.data());

        //Send local array over socket
        client->send(
            localArray.data(),
            sizeof(shared_fft_t::value_type) * shared_fft_t::size);

    }

}

int main(int argc, char** argv) {

    //Register signal handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    int opt;
    bool forever = false;
    std::string ipaddr = "127.0.0.1";

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
        
        std::unique_ptr<Socket::Client> socket;

        //Create socket to send data
        //Keep looping on connection error in case server has not been set up
        while(!quit) {
            try {
                socket = std::make_unique<Socket::Client>(ipaddr.c_str(), PORTNO, socket_send);
            }
            catch (Socket::Connection_Error &) {
                Poller poller (1000); //Wait 1 second between connection attempts
                std::cerr << "Connection to server failed, reconnecting ..." << std::endl;
                continue;
            }
            break;
        }
        

        //FFT loop
        for(int i = 0; i < 1000 && !quit; forever ? i : i++) {
            Poller poller(WINDOW_LATENCY_MS);
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
