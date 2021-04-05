#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include "../fft2midi/channelbroker.h"

ChannelBroker channel_broker {false}; //Do not print ports
std::atomic<int> thread_counter;
std::mutex cout_mtx;

void test(int thread_count, bool drum) {
    thread_counter++;
    while(thread_counter < thread_count) {}
    Channel channel (channel_broker, drum);
    cout_mtx.lock();
    if(drum) std::cout << "I asked for the drum. ";
    std::cout << "I got channel " << channel.get_channel() << std::endl;
    cout_mtx.unlock();
}


int main(int argc, char * argv[]) {

    thread_counter = 0;
    std::vector<std::thread> threads;
    threads.emplace_back(test, 4, true);
    threads.emplace_back(test, 4, false);
    threads.emplace_back(test, 4, true);
    threads.emplace_back(test, 4, false);
    while (thread_counter < 4) {}
    threads.emplace_back(test, 8, true);
    threads.emplace_back(test, 8, false);
    threads.emplace_back(test, 8, true);
    threads.emplace_back(test, 8, false);
    for (std::thread & t : threads) {
        if(t.joinable()) t.join();
    }




}