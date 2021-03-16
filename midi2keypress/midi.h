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

#endif /* MIDI_H */


