#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include "../fft2midi/midi.h"
#include "../fft2midi/channel.h"

MidiStream ms; //Do not print ports
std::mutex cout_mtx;

//Barrier implementation is dumb but it works
std::atomic<int> b1;
std::atomic<int> b2;
void barrier(std::atomic<int> & b) {
    b++;
    while (b < 4) {}
}

void test(bool drum) {
    barrier(b1);
    MidiChannel channel (ms, drum);
    cout_mtx.lock();
    //std::cout << "I am thread " << local_counter << ". ";
    if(drum) std::cout << "I asked for the drum. ";
    std::cout << "I got channel " << channel.get_channel() << std::endl;
    cout_mtx.unlock();
    barrier(b2);
}

void make_threads(void) {
    b1 = 0;
    b2 = 0;
    std::vector<std::thread> threads;
    
    threads.emplace_back(test, true);
    threads.emplace_back(test, false);
    threads.emplace_back(test, true);
    threads.emplace_back(test, false);
    
    for (std::thread & t : threads) {
        if(t.joinable()) t.join();
    }
}


int main(int argc, char * argv[]) {

    make_threads();
    make_threads();

    return 0;

}
