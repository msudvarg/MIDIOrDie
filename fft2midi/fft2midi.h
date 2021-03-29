#pragma once

#include "tone.h"
#include "midi.h"
#include "../fft/fft.h"

class Desynthesizer {

private:
    MidiStream ms;
    FreqList old_peaks;
    Tone tone;
    bool all;

public:

    //FFT::Shared_Array_t::array_type fft_data;
    FFT::Shared_Array_t::array_type & fft_data() { return tone.interval; }

    void run();

    Desynthesizer(int port, unsigned int channel, bool _all);
    ~Desynthesizer() = default;

};
