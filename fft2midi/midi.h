#ifndef MIDI_H
#define MIDI_H

#include <cmath>
#include "../rtmidi/RtMidi.h"
#include "channelbroker.h"

class MidiStream {
public:
  void Send(unsigned char note, bool on, unsigned channel);
  void ChangeInstrument(unsigned char instrument, unsigned channel);

  void freeChannel (unsigned int channel) { broker.freeChannel(channel); }
  bool getChannel(unsigned int & channel, bool drum) { return broker.getChannel(channel, drum); }

  MidiStream(int port = 0);


private:
  RtMidiOut midiout;
  ChannelBroker broker;
  
  // frequency = 440 * 2^((n-69)/12)
  // n = lg(f/440) * 12 + 69
  static constexpr float LG_440 = 8.78135971352466;

  static constexpr unsigned char NOTE_ON = 0x90;
  static constexpr unsigned char NOTE_OFF = 0x80;
  static constexpr unsigned char PC_CHANGE = 0xC0;
};

static inline bool InMidiRange(int freq) {
  return freq > 64 and freq < 4435;
}

static inline unsigned char Freq2Midi(int freq) {
  return (unsigned char) (log2((float) freq / 440) * 12) + 69;
}

#endif /* MIDI_H */
