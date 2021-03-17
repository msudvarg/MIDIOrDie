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

char Freq2Midi(float freq);

#endif /* MIDI_H */


