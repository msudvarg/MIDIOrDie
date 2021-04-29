#pragma once

#include "tone.h"
#include "channel.h"

class Desynthesizer {

private:
    MidiChannel & channel;
    NotesList old_notes;
    Tone tone;
    bool all;
    bool hillclimb;

public:

    shared_fft_t::array_type & fft_data() { return tone.interval; }

    void run(ModelLoader &model);

  Desynthesizer(MidiChannel & _channel, bool _all, bool _hillclimb);
    ~Desynthesizer() = default;

};
