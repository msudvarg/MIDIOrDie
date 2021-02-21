
#include <string.h>
#include <complex>
#include <cmath>
#include <iostream>
#include <signal.h>
#include <random>
#include "Shared_Memory.h"
#include "common.h"

sig_atomic_t quit = 0;

void sigint_handler(int signum) {
    quit = 1;
}

int main(int argc, char * argv[]) {

    
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);
    
    //Create shared memory for visualization
    Shared_Memory<Shared_Buffer> sharedBuffer {"finalOutputBuffer"}; 
    std::cout << "Shared memory size: " << sizeof(Shared_Buffer) << std::endl;
    
    sharedBuffer->lock_sequence = 0;

    memset(sharedBuffer->finalOutputBuffer, 0, sizeof(finalOutputBuffer));

    std::default_random_engine g;
    std::uniform_real_distribution<double> d;


    while(!quit) {

        for (int i = 0; i < ROLLING_WINDOW_SIZE; ++i) {
            finalOutputBuffer[i] = d(g);
        }
        
        // TODO: Dump finalOutputBuffer here for visualization
        sharedBuffer->lock_sequence++;
        memcpy(sharedBuffer->finalOutputBuffer, finalOutputBuffer, sizeof(finalOutputBuffer));
        sharedBuffer->lock_sequence++;

        sleep(1);
    }
    
    return 0;

}