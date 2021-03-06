#pragma once

#include <exception>

#include "midi.h"

class MidiChannel {

private:
    MidiStream & ms;
    unsigned int channel;

public:

    struct No_Channel : std::exception {
        const char* what() const noexcept {return "No available channels!\n";}
    };    

    unsigned int get_channel() const { return channel; }
    void Send(unsigned char note, bool on) { ms.Send(note, on, channel); }
    void ChangeInstrument(unsigned char instrument) { ms.ChangeInstrument(instrument, channel); }

    MidiChannel(MidiStream & _ms, bool _drum);

    ~MidiChannel() {
        ms.freeChannel(channel);
    }

};