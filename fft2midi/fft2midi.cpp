#include <iostream>
#include <algorithm>
#include <unistd.h>

#include "fft2midi.h"

bool done = false;

Desynthesizer::Desynthesizer(MidiChannel & _channel, bool _all, bool _hillclimb, std::vector<float> calib) :
  channel (_channel),
  all (_all),
  hillclimb (_hillclimb),
  tone(calib)
{
}

void Desynthesizer::run(ModelLoader &model) {
  NotesList notes;

  if (hillclimb) {
    notes = tone.HillClimbConstrained();
  } else {
    notes = tone.ExtractSignatures(model);
  }

    
    //Print extracted note information
    for(int n : notes) {
      std::cout << tone.GetNoteName(n) << " ";
    }
    std::cout << std::endl;
    

    if (all) {
      // Play all the pitches found in the fft
      for (int note : FreqDifference(notes, old_notes)) {
        if (InMidiRange(note)) {
          channel.Send(note, true);
        }
      }

      for (int note : FreqDifference(old_notes, notes)) {
        if (InMidiRange(note)) {
          channel.Send(note, false);
        }
      }
    } else {
      // Just play the fundamental frequency
      if (notes.size() > 0) {
        int note = notes[0];
        if (old_notes.size() == 0) {
          if (InMidiRange(note)) {
            channel.Send(note, true);
          }
        } else if (old_notes[0] != note) {
          if (InMidiRange(note)) {
            channel.Send(note, true);
          }
          if (InMidiRange(old_notes[0])) {
            channel.Send(old_notes[0], false);
          }
        }
      } else if (old_notes.size() > 0) {
        if (InMidiRange(old_notes[0])) {
          channel.Send(old_notes[0], false);
        }
      }
    }
    old_notes = notes;

}



