#include <iostream>
#include <algorithm>
#include <unistd.h>

#include "fft2midi.h"

bool done = false;

Desynthesizer::Desynthesizer(int port, unsigned int channel, bool _all, bool _hillclimb) :
  ms (port),
  all (_all),
  hillclimb (_hillclimb),
  tone ("../signature_extraction_model/tf_model/", "../signature_extraction_model/acoustic_oren_calib_row48.npy")    // Make the calib file not hard-coded
{
  ms.ChangeChannel(channel);
}

void Desynthesizer::run() {
  NotesList notes;

  if (hillclimb) {
    notes = tone.HillClimbConstrained();
  } else {
    notes = tone.ExtractSignatures();
  }

    for(int n : notes) {
      std::cout << tone.GetNoteName(n) << " ";
    }
    std::cout << std::endl;

    if (all) {
      // Play all the pitches found in the fft
      for (int note : FreqDifference(notes, old_notes)) {
        if (InMidiRange(note)) {
          ms.Send(note, true);
        }
      }

      for (int note : FreqDifference(old_notes, notes)) {
        if (InMidiRange(note)) {
          ms.Send(note, false);
        }
      }
    } else {
      // Just play the fundamental frequency
      if (notes.size() > 0) {
        int note = notes[0];
        if (old_notes.size() == 0) {
          if (InMidiRange(note)) {
            ms.Send(note, true);
          }
        } else if (old_notes[0] != note) {
          if (InMidiRange(note)) {
            ms.Send(note, true);
          }
          if (InMidiRange(old_notes[0])) {
            ms.Send(old_notes[0], false);
          }
        }
      } else if (old_notes.size() > 0) {
        if (InMidiRange(old_notes[0])) {
          ms.Send(old_notes[0], false);
        }
      }
    }
    old_notes = notes;

}



