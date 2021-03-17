#ifndef MIDI_H
#define MIDI_H

#include <RtMidi.h>

class MidiStream {
public:
  void Init();
  void Send(unsigned char note, bool on);
private:
  RtMidiOut midiout;
};

static inline bool InMidiRange(int freq) {
  return freq > 64 and freq < 4435;
}

unsigned char Freq2Midi(int freq);

#endif /* MIDI_H */
