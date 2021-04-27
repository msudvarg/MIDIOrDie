#pragma once

#include "tone.h"
#include "channel.h"

class Desynthesizer {

private:
    MidiChannel & channel;
    FreqList old_peaks;
    Tone tone;
    bool all;
    bool hillclimb;

public:

    shared_fft_t::array_type & fft_data() { return tone.interval; }

    void run();

  Desynthesizer(MidiChannel & _channel, bool _all, bool _hillclimb);
    ~Desynthesizer() = default;

};
