#include "midi.h"

#include <vector>

#include <math.h>

void MidiStream::Init() {
  midiout.openPort(0);
}

void MidiStream::Send(unsigned char note, bool on) {
  std::vector<unsigned char> buf;
  if (on) {
    buf.push_back(144);
  } else  {
    buf.push_back(128);
  }
  buf.push_back(note);
  buf.push_back(0);
  midiout.sendMessage(&buf);
}
// frequency = 440 * 2^((n-68)/12)
// n = lg(f/440) * 12 + 68
char Freq2Midi(float freq) {
  return ((int) (log2f(freq / 440.0f) * 12) + 68);
}
