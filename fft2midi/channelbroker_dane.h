#pragma once

#include <exception>
#include <mutex>

// God almighty this was overengineered
// Also wrong because _Find_first returns the first _set_ bit

class ChannelBroker {
    
private:
    std::mutex m;
    bool channels[16];

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

// Local Variables:
// c-basic-offset: 4
// End:
