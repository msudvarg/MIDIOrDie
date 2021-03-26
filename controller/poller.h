#pragma once

//RAII class for polling
//Instantiate at beginning of scope with specified polling rate
//Destructor uses sleep_until so that leaving scope causes a pause
//Useful at the beginning of a loop body

#include <thread>
#include <chrono>
#include <ctime>

class Poller {

using clock_type = std::chrono::steady_clock;

private:

    std::chrono::time_point<clock_type> init_time;
    std::chrono::time_point<clock_type> wake_time;

public:


    Poller(int ms) {
        init_time = clock_type::now();
        //milliseconds _ms (ms);
        wake_time = init_time + std::chrono::milliseconds(ms);
    }

    ~Poller() {
        std::this_thread::sleep_until(wake_time);
    }
};