#include "midi.h"

#include <iostream>
#include <math.h>

#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define PC_CHANGE 0xC0
#define PITCH_WHEEL 0xE0

MidiStream::MidiStream(int port) {
  //port=0 as default in declaration
  midiout.openPort(port);
  std::cout << "Connected to MIDI port: " << port << std::endl;
}

void MidiStream::Send(unsigned char note, bool on, unsigned channel, unsigned id) {
  constexpr unsigned msgsize = 3;
  unsigned char buf[msgsize];

  //MIDI latency
  buf[0] = PITCH_WHEEL | channel;
  
  buf[1] = id % 128;
  buf[2] = (id/128) % 128;

  {
    std::lock_guard<std::mutex> lk {send_mtx};
    send_times.log();
    midiout.sendMessage(buf, msgsize);
  }


  //Send note info
  if (on) {
    buf[0] = NOTE_ON | channel;
  } else  {
    buf[0] = NOTE_OFF | channel;
  }
  buf[1] = note;
  buf[2] = 127;

  //Not sure if sendMessage is thread safe
  {
    std::lock_guard<std::mutex> lk {send_mtx};
    midiout.sendMessage(buf, msgsize);
  }
}

void MidiStream::ChangeInstrument(unsigned char instrument, unsigned channel) {
  constexpr unsigned msgsize = 2;
  unsigned char buf[msgsize];
  buf[0] = PC_CHANGE | channel;
  buf[1] = instrument;
  midiout.sendMessage(buf, msgsize);
}

MidiStream::~MidiStream() {
  send_times.print("midi_send_times.txt");
}