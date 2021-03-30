#include <iostream>
#include <algorithm>
#include <unistd.h>

#include "fft2midi.h"

bool done = false;

Desynthesizer::Desynthesizer(int port, unsigned int channel, bool _all, bool _hillclimb) :
  ms (port),
  //tone (FFT::WINDOW_SIZE, FFT::OUTPUT_FFT_MAX_HZ),
  all (_all),
  hillclimb (_hillclimb)
{
  ms.ChangeChannel(channel);

  tone.DummySignature();
}

void Desynthesizer::run() {
  FreqList peaks;

  if (hillclimb) {
    peaks = tone.Hillclimb();
  } else {
    peaks = tone.ExtractSignatures();
  }

    for(int n : peaks) {
      std::cout << tone.GetNoteName(n) << " ";
    }
    std::cout << std::endl;

    if (all) {
      // Play all the pitches found in the fft
      for (int freq : FreqDifference(peaks, old_peaks)) {
        if (InMidiRange(freq)) {
          ms.Send(Freq2Midi(freq), true);
        }
      }

      for (int freq : FreqDifference(old_peaks, peaks)) {
        if (InMidiRange(freq)) {
          ms.Send(Freq2Midi(freq), false);
        }
      }
    } else {
      // Just play the fundamental frequency
      if (peaks.size() > 0) {
        int freq = peaks[0];
        if (old_peaks.size() == 0) {
          if (InMidiRange(freq)) {
            ms.Send(Freq2Midi(freq), true);
          }
        } else if (old_peaks[0] != freq) {
          if (InMidiRange(freq)) {
            ms.Send(Freq2Midi(freq), true);
          }
          if (InMidiRange(old_peaks[0])) {
            ms.Send(Freq2Midi(old_peaks[0]), false);
          }
        }
      } else if (old_peaks.size() > 0) {
        if (InMidiRange(old_peaks[0])) {
          ms.Send(Freq2Midi(old_peaks[0]), false);
        }
      }
    }
    old_peaks = peaks;

}



