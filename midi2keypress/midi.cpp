#include "midi.h"

#include <vector>

#include <math.h>
// #include <string.h>

void MidiStream::Init() {
  for (int i = 0; i < midiout.getPortCount(); i++) {
    std::cout << i << ": " << midiout.getPortName(i) << std::endl;
  }
  midiout.openPort(1);
}

void MidiStream::Send(unsigned char note, bool on) {
  std::vector<unsigned char> buf;
  if (on) {
    buf.push_back(144);
  } else  {
    buf.push_back(128);
  }
  buf.push_back(note);
  buf.push_back(100);
  midiout.sendMessage(&buf);
}
// frequency = 440 * 2^((n-69)/12)
// n = lg(f/440) * 12 + 69

#define LG_440 8.78135971352466f

unsigned char Freq2Midi(int freq) {
  // int log2_freq = ffs(freq) - 1;
  // return (log2_freq * 12 - LG_440 * 12) + 69;
  return (unsigned char) (log2((double) freq / 440) * 12) + 69;
}
