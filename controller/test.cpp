
#include <string.h>
#include <complex>
#include <cmath>
#include <iostream>
#include <signal.h>
#include <random>
#include <chrono>
#include "common.h"
#include "Shared_Memory.h"
#include "socket_client.h"
#include "socket_manifest.h"
#include "socket_send.h"
#include "shared_buffer.h"

Shared_Array<double,ROLLING_WINDOW_SIZE> sharedArray;

sig_atomic_t quit = 0;
double finalOutputBuffer[ROLLING_WINDOW_SIZE];

void sigint_handler(int signum) {
    quit = 1;
}

int main(int argc, char * argv[]) {

    
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    //Create socket to send data
    Socket_Client socket {IPADDR, PORTNO, socket_send};
    
    //Create shared memory for visualization
    Shared_Memory<Shared_Buffer> sharedBuffer {"finalOutputBuffer"}; 
    std::cout << "Shared memory size: " << sizeof(Shared_Buffer) << std::endl;
    
    sharedBuffer->lock_sequence = 0;

    memset(sharedBuffer->fftData, 0, sizeof(finalOutputBuffer));

    std::default_random_engine g;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;
    g.seed(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    std::uniform_real_distribution<double> d;

    while(!quit) {

        for (int i = 0; i < ROLLING_WINDOW_SIZE; ++i) {
            finalOutputBuffer[i] = d(g);
        }
        
        
        // TODO: Dump finalOutputBuffer here for visualization
        sharedBuffer->lock_sequence++;
        memcpy(sharedBuffer->fftData, finalOutputBuffer, sizeof(finalOutputBuffer));
        sharedBuffer->lock_sequence++;
    }
    
    return 0;

}
