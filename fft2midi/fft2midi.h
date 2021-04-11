#pragma once

#include "tone.h"

class Desynthesizer {

private:
    MidiStream ms;
    NotesList old_notes;
    Tone tone;
    bool all;
    bool hillclimb;

public:

    shared_fft_t::array_type & fft_data() { return tone.interval; }

    void run();

  Desynthesizer(int port, unsigned int channel, bool _all, bool _hillclimb);
    ~Desynthesizer() = default;

};
