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
#include "../cnpy/cnpy.h"

ChannelBroker channel_broker;

int port = 0;
bool drum = false;
bool all = false;
bool hillclimb = false;
ModelLoader model("../signature_extraction_model/tf_model/");
std::vector<float> dummy_calib;

//Destructors not correctly called if program interrupted
//Use a signal handler and quit flag instead
sig_atomic_t quit = 0;
void sigint_handler(int signum) {
    quit = 1;
}


void socket_recv(Socket::Connection * client) {

    Channel channel(channel_broker, drum);

    std::cout << "Connecting to MIDI port: " << port << " ..." << std::endl;
    Desynthesizer desynth {port, channel.get_channel(), all, hillclimb, dummy_calib};
    shared_fft_t::array_type & fft_data = desynth.fft_data();

    //Loop and do stuff
    while(client->isrunning() && !quit) {
        
        Poller poller(WINDOW_LATENCY_MS);

        //Read from socket into Desynth object. Contents can be found in desynth.tone.interval
        client->recv(
            fft_data.data(),
            sizeof(shared_fft_t::value_type) * shared_fft_t::size);

        desynth.run(model);

    }

}

int main(int argc, char** argv) {

    //Register signal handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    //Get command-line options
    int opt;

    // TODO: Replace this with a loop that asks a new player to calibrate their guitar, or 
	cnpy::NpyArray arr = cnpy::npy_load("../signature_extraction_model/acoustic_oren_calib_row48.npy");
	assert(arr.shape.size() == 2 && arr.shape[0] == 4 && arr.shape[1] == OUTPUT_FFT_SIZE);
    int i = 0;
    dummy_calib.resize(4 * OUTPUT_FFT_SIZE);
    for(auto v : arr.as_vec<double>()) {
        dummy_calib[i] = v;
        i++;
    }

  
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
