#include <iostream>
#include "../rtmidi/RtMidi.h"
#include "channelbroker.h"

bool ChannelBroker::getChannel(unsigned int & channel, bool drum) {

    channel = channel_count; //Unassigned

    //Lock broker
    std::lock_guard<std::mutex> lk {m};

    //Try to obtain drum
    if (drum) {
        //Drum available
        if (!channels[drum_channel]) {
            channel = drum_channel;
        }

        //Drum unavailable
        else drum = false;
    }

    //Obtain channel
    if (!drum) {
        for (int i = 0; i < channel_count; ++i) {
            if(!channels[i]) {
                channel = i;
                break;
            }
        }
    }

    //Channel assigned
    if(channel < channel_count) {
        channels[channel] = true;
        return true;
    }

    //No free channels
    else return false;

}

void ChannelBroker::freeChannel (unsigned int channel) {

    //Lock broker
    std::lock_guard<std::mutex> lk {m};

    //Remove channel
    channels[channel] = false;
}

ChannelBroker::ChannelBroker(bool print_ports) {
    //Initialize channel container elements
    for (int i = 0; i < channel_count; ++i) {
        channels[i] = false;
    }

    if(print_ports) {
        //Print port info
        RtMidiOut midiout;
        unsigned int port_count = midiout.getPortCount();
        std::cout << "There are " << port_count << " MIDI ports" << std::endl;
        for (unsigned int i = 0; i < port_count; i++) {
            std::cout << i << ": " << midiout.getPortName(i) << std::endl;
        }
    }
}

Channel::Channel(ChannelBroker & _broker, bool _drum) :
    broker (_broker)
{
    if (!broker.getChannel(channel, _drum)) throw No_Channel{};
}