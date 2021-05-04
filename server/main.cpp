#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include "portaudio.h"

#include "../include/manifest.h"
#include "../socket/socket.h" //Socket wrapper
#include "../include/shared_array.h" //Thread-safe array
#include "../include/poller.h"
#include "../fft2midi/fft2midi.h"
#include "../include/timing.h"

std::unique_ptr<MidiStream> ms;

int port = 0;
bool drum = false;
bool all = false;
bool hillclimb = false;
ModelLoader model("../signature_extraction_model/tf_model/");

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}


void socket_recv(Socket::Connection * client) {

    MidiChannel channel(*ms, drum);
    
    Desynthesizer desynth {channel, all, hillclimb, model};
    shared_fft_t::array_type & fft_data = desynth.fft_data();
    
    TimingLog<Microseconds,200> desynth_times(TimingLogType::StartStop);
    TimingLog<Microseconds,200> socket_times(TimingLogType::StartStop);
    TimingLog<Microseconds,200> jitter(TimingLogType::AllTimestamps);

    //Tell client ready
    char ready = 1;
    client->send(&ready, 1);

    //Loop and do stuff
    while(client->isrunning() && !quit) {
        
        Poller poller(WINDOW_LATENCY_MS);

        socket_times.log();
        //Read from socket into Desynth object. Contents can be found in desynth.tone.interval
        client->recv(
            fft_data.data(),
            sizeof(shared_fft_t::value_type) * shared_fft_t::size);
        socket_times.log();

        //Send back to client for timing data
        client->send(&ready, 1);

        desynth_times.log();
        desynth.run(model);
        desynth_times.log();
        
        jitter.log();

    }

    desynth_times.print("desynth_times.txt");
    socket_times.print("socket_send_times.txt");
    jitter.print("controller_jitter_times.txt");

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

        //Construct MidiStream on specified port
    	std::cout << "Connecting to MIDI port: " << port << " ..." << std::endl;
        ms = std::make_unique<MidiStream>(port);
        
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
