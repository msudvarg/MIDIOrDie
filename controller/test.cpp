
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <complex>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>

#include "common.h" //FFT parameters and shared data structure
#include "shared_memory.h" //Shared memory wrapper
#include "socket/socket.h" //Socket wrapper
#include "socket_helpers.h" //Functions to pass to socket connections
#include "shared_array.h" //Thread-safe array

//Thread-safe array to send FFT data over socket
Shared_Array<double,ROLLING_WINDOW_SIZE> sharedArray;
//Local array
double finalOutputBuffer[ROLLING_WINDOW_SIZE];

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
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

    //Create socket to send data
    Socket::Client socket {ipaddr.c_str(), PORTNO, socket_send};
    
    //Create shared memory for visualization
    Shared_Memory<Shared_Buffer> sharedBuffer {"finalOutputBuffer"}; 
    std::cout << "Shared memory size: " << sizeof(Shared_Buffer) << std::endl;

    //RNG for random FFT histogram values
    std::default_random_engine g;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;
    g.seed(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    std::uniform_real_distribution<double> d;

    while(!quit) {
        
        //Generate random FFT histogram values
        for (int i = 0; i < ROLLING_WINDOW_SIZE; ++i) {
            finalOutputBuffer[i] = d(g) * 1000;
        }

        //copy random values to shared array
        sharedArray.write(finalOutputBuffer);
        
        //Write to shared memory
        sharedBuffer->fftData.write(finalOutputBuffer);

    }
    
    return 0;

}
