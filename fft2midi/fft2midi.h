#pragma once

#include "tone.h"
#include "midi.h"
#include "../fft/fft.h"

class Desynthesizer {

private:
    MidiStream ms;
    FreqList old_peaks;
    Tone tone;
    int port;
    bool drum;
    bool all;

public:

    FFT::Shared_Array_t::array_type fft_data;

    void run();

    Desynthesizer(int _port, bool _drum, bool _all);
    ~Desynthesizer() = default;

};
