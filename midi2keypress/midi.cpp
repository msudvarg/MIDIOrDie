#include "midi.h"

#include <vector>

#include <math.h>
// #include <string.h>

#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define PC_CHANGE 0xC0

void MidiStream::Init() {
  Init(0);
}

void MidiStream::Init(int port) {
  for (int i = 0; i < midiout.getPortCount(); i++) {
    std::cout << i << ": " << midiout.getPortName(i) << std::endl;
  }
  midiout.openPort(port);
}

void MidiStream::Send(unsigned char note, bool on) {
  std::vector<unsigned char> buf;
  if (on) {
    buf.push_back(NOTE_ON | channel);
  } else  {
    buf.push_back(NOTE_OFF | channel);
  }
  buf.push_back(note);
  buf.push_back(127);
  midiout.sendMessage(&buf);
}

void MidiStream::ChangeInstrument(unsigned char instrument) {
  std::vector<unsigned char> buf;
  buf.push_back(PC_CHANGE | channel);
  buf.push_back(instrument);
  midiout.sendMessage(&buf);
}

void MidiStream::ChangeChannel(unsigned char channel) {
  this->channel = channel;
}

// frequency = 440 * 2^((n-69)/12)
// n = lg(f/440) * 12 + 69

#define LG_440 8.78135971352466f

unsigned char Freq2Midi(int freq) {
  // int log2_freq = ffs(freq) - 1;
  // return (log2_freq * 12 - LG_440 * 12) + 69;
  return (unsigned char) (log2((double) freq / 440) * 12) + 69;
}
