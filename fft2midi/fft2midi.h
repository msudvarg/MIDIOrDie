#pragma once

#include "tone.h"
#include "midi.h"

class Desynthesizer {

private:
    MidiStream ms;
    FreqList old_peaks;
    Tone tone;
    bool all;
    bool hillclimb;

public:

    shared_fft_t::array_type & fft_data() { return tone.interval; }

    void run();

  Desynthesizer(int port, unsigned int channel, bool _all, bool _hillclimb);
    ~Desynthesizer() = default;

};
