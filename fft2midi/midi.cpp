#include "midi.h"

#include <vector>
#include <iostream>

#include <math.h>
// #include <string.h>

#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define PC_CHANGE 0xC0

MidiStream::MidiStream(int port) {
  //port=0 as default in declaration
  midiout.openPort(port);
  std::cout << "Connected to MIDI port " << port << std::endl;
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
