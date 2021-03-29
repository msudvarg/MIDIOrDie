#pragma once

#include <bitset>
#include <exception>
#include <mutex>


class ChannelBroker {
    
private:
    static constexpr unsigned int channel_count = 16;
    std::bitset<channel_count> channels;
    std::mutex m;

public:
    static constexpr unsigned int drum_channel = 9;
    
    bool getChannel(unsigned int & channel, bool drum);
    void freeChannel (unsigned int channel);

    ChannelBroker();
};

class Channel {

private:
    ChannelBroker & broker;
    unsigned int channel;

public:

    struct No_Channel : std::exception {
        const char* what() const noexcept {return "No available channels!\n";}
    };    

    unsigned int get_channel() const { return channel; }

    Channel(ChannelBroker & _broker, bool _drum);

    ~Channel() {
        broker.freeChannel(channel);
    }

};