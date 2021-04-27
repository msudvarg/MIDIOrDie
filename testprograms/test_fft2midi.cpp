
#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <thread>

#include "../include/manifest.h"
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
#include "../fft2midi/fft2midi.h"
#include "../localcontroller/localcontroller.h"

bool forever = false;

int port = 0;
bool drum = false;
bool all = false;
bool hillclimb = false;

LocalController lc;
std::unique_ptr<MidiStream> ms;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}

void run_desynth() {

    MidiChannel channel(*ms, drum);

    Desynthesizer desynth {channel, all, hillclimb};

    shared_fft_t::array_type localArray;

    //Desynth loop
    for(int i = 0; i < 1000 && !quit; forever ? i : i++) {
        
        Poller poller(WINDOW_LATENCY_MS);

        lc.GetData(localArray.data());

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
    while((opt = getopt(argc, argv, "fadpc:")) != -1) {
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
        case 'c':
            hillclimb = true;
            break;
        }
    }

    //Exception handling

    try {

        //Construct MidiStream on specified port
        ms = std::make_unique<MidiStream>(port);

        std::thread thread_desynth {run_desynth};
        thread_desynth.join();

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
