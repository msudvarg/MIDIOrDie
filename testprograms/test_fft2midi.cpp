
#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <thread>

#include "../include/manifest.h"
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
#include "../fft/fft.h"
#include "../fft2midi/fft2midi.h"

bool forever = false;

int port = 0;
bool drum = false;
bool all = false;

FFT fft;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}

void run_fft() {
    
    //FFT loop
    for(int i = 0; i < 1000 && !quit; forever ? i : i++) {

        Poller poller(polling_freq);

        fft.run();
    }
}

void run_desynth() {

    Desynthesizer desynth {port, drum, all};

    //Desynth loop
    for(int i = 0; i < 1000 && !quit; forever ? i : i++) {

        Poller poller(polling_freq);

        //Copy FFT data to desynth array
        desynth.fft_data() = fft.read();

        desynth.run();

    }

}


int main(int argc, char** argv) {

    //Register signal handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    int opt;

    //Get command-line options
    while((opt = getopt(argc, argv, "fadp:")) != -1) {
        switch(opt) {

        //Run forever
        case 'f':
            forever = true;
            break;

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

        fft.init();

        std::thread thread_fft {run_fft};
        std::thread thread_desynth {run_desynth};
        thread_fft.join();
        thread_desynth.join();
        
        fft.end();

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
