#pragma once

#include <mutex>

class ChannelBroker {
    
private:
    static constexpr unsigned int channel_count = 16;
    bool channels[channel_count];
    std::mutex m;

public:
    static constexpr unsigned int drum_channel = 9;
    
    bool getChannel(unsigned int & channel, bool drum);
    void freeChannel (unsigned int channel);

    ChannelBroker(bool print_ports = true);
};