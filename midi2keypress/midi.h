#ifndef MIDI_H
#define MIDI_H

#include <RtMidi.h>

class MidiStream {
public:
  void Init();
  void Init(int port);
  void Send(unsigned char note, bool on);
  void ChangeInstrument(unsigned char instrument);
  void ChangeChannel(unsigned char channel);
private:
  RtMidiOut midiout;
  unsigned char channel = 0;
};

static inline bool InMidiRange(int freq) {
  return freq > 64 and freq < 4435;
}

unsigned char Freq2Midi(int freq);

#endif /* MIDI_H */
