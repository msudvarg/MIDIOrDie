#include <iostream>
#include <algorithm>
#include <unistd.h>

#include "localcontroller.h"
#include "fft2midi.h"

bool done = false;

Desynthesizer(int _port, bool _drum, bool _all) :
  port (_port),
  drum (_drum),
  all (_all),
  tone (WINDOW_SIZE, OUTPUT_FFT_MAX_HZ)
{
  ms.init(port);
  if (drum) {
    ms.ChangeChannel(9);
  }

  tone.DummySignature();
}

void Desynthesizer::run() {

    //Change to socket getter
    //lc.GetData(tone.interval, tone.raw_audio);
  
    FreqList peaks = tone.ExtractSignatures();

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



