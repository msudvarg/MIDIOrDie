
#include <iostream>
#include "../rtmidi/RtMidi.h"
#include "channelbroker.h"

bool ChannelBroker::getChannel(unsigned int & channel, bool drum) {

    //Lock broker
    std::lock_guard<std::mutex> lk {m};

    //Check for free channel
    if(channels.all()) return false;

    //Try to obtain drum
    if (drum) {
        //Drum available
        if (!channels.test(drum_channel)) {
            channel = drum_channel;
        }

        //Drum unavailable
        else drum = false;
    }

    //Obtain channel
    if (!drum) {
        channel = channels._Find_first();
    }

    channels.set(channel);

    return true;

}

void ChannelBroker::freeChannel (unsigned int channel) {

    //Lock broker
    std::lock_guard<std::mutex> lk {m};

    //Remove channel
    channels.reset(channel);
}

ChannelBroker::ChannelBroker() {
    RtMidiOut midiout;
    unsigned int port_count = midiout.getPortCount();
    for (unsigned int i = 0; i < port_count; i++) {
        std::cout << i << ": " << midiout.getPortName(i) << std::endl;
    }
}

Channel::Channel(ChannelBroker & _broker, bool _drum) :
    broker (_broker)
{
    if (!broker.getChannel(channel, _drum)) throw No_Channel{};
}