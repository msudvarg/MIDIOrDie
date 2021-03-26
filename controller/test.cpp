#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <memory>
#include "portaudio.h"

#include "../include/manifest.h"
#include "../socket/socket.h" //Socket wrapper
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
#include "fft.h"

//Specific to test.cpp
#include <random>

//Thread-safe array to send FFT data over socket
Shared_Array<double,FFT::WINDOW_SIZE> sharedArray;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}


void socket_send(Socket::Connection * client) {
    
    Poller poller(polling_freq);

    //Copy shared array to local array
    FFT::Shared_Array_t::array_type localArray = sharedArray.read();

    //Send local array over socket
    client->send(
        localArray.data(),
        sizeof(FFT::Shared_Array_t::value_type) * FFT::Shared_Array_t::size);

}

int main(int argc, char * argv[]) {

    //Register signal handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    int opt;
    std::string ipaddr = "127.0.0.1";

    //Get command-line options
    while((opt = getopt(argc, argv, "i:")) != -1) {
        switch(opt) {

        //IP Address
        case 'i':
            if(optarg) ipaddr.assign(optarg);
            break;

        default:
            std::cerr << "Usage: " << argv[0] << " [-i0.0.0.0]\n" << std::endl;
        }
    }

    //Exception handling

    try {

        //RNG for random FFT histogram values
        std::default_random_engine g;
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        using std::chrono::system_clock;
        g.seed(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
        std::uniform_real_distribution<double> d;
        
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
        
        //Local array
        double finalOutputBuffer[FFT::WINDOW_SIZE];

        while(!quit) {
            
            //Generate random FFT histogram values
            for (int i = 0; i < FFT::WINDOW_SIZE; ++i) {
                finalOutputBuffer[i] = d(g) * 1000;
            }

            //copy random values to shared array
            sharedArray.write(finalOutputBuffer);

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
