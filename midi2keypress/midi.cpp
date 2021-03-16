#include "midi.h"

#include <vector>

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
