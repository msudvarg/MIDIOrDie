#pragma once

#include "tone.h"
#include "midi.h"
#include "portbroker.h"
#include "../fft/fft.h"

class Desynthesizer {

private:
    MidiStream ms;
    FreqList old_peaks;
    Tone tone;
    bool all;

public:

    FFT::Shared_Array_t::array_type fft_data;

    void run();

    Desynthesizer(const Port & port, bool _all);
    Desynthesizer(unsigned int port, bool drum, bool _all);
    ~Desynthesizer() = default;

};
