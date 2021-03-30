
#include <iostream>
#include "../rtmidi/RtMidi.h"
#include "channelbroker.h"

bool ChannelBroker::getChannel(unsigned int & channel, bool drum) {

    //Lock broker
    m.lock();

    // Only one drum channel
    if (drum) {
	if (channels[drum_channel]) {
	    m.unlock();
	    return false;
	} else {
	    channels[drum_channel] = true;
	    m.unlock();
	    return true;
	}
    }

    // Find an available channel
    for(int i = 0; i < 16; i++) {
	if (!channels[i]) {
	    m.unlock();
	    channel = i;
	    return true;
	}
    }

    // No available channels
    m.unlock();
    return false;
}

void ChannelBroker::freeChannel (unsigned int channel) {

    //Lock broker
    m.lock();

    channels[channel] = false;

    m.unlock();
}

ChannelBroker::ChannelBroker() {
    RtMidiOut midiout;
    unsigned int port_count = midiout.getPortCount();
    std::cout << "There are " << port_count << " MIDI ports" << std::endl;
    for (unsigned int i = 0; i < port_count; i++) {
        std::cout << i << ": " << midiout.getPortName(i) << std::endl;
    }
}

Channel::Channel(ChannelBroker & _broker, bool _drum) :
    broker (_broker)
{
    if (!broker.getChannel(channel, _drum)) throw No_Channel{};
}

// Local Variables:
// c-basic-offset: 4
// End:
