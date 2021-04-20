#include "midi.h"

#include <iostream>
#include <math.h>

#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define PC_CHANGE 0xC0

MidiStream::MidiStream(int port) {
  //port=0 as default in declaration
  midiout.openPort(port);
  std::cout << "Connected to MIDI port: " << port << std::endl;
}

void MidiStream::Send(unsigned char note, bool on, unsigned channel) {
  constexpr unsigned msgsize = 3;
  unsigned char buf[msgsize];
  if (on) {
    buf[0] = NOTE_ON | channel;
  } else  {
    buf[0] = NOTE_OFF | channel;
  }
  buf[1] = note;
  buf[2] = 127;
  midiout.sendMessage(buf, msgsize);
}

void MidiStream::ChangeInstrument(unsigned char instrument, unsigned channel) {
  constexpr unsigned msgsize = 2;
  unsigned char buf[msgsize];
  buf[0] = PC_CHANGE | channel;
  buf[1] = instrument;
  midiout.sendMessage(buf, msgsize);
}