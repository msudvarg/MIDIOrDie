#include <iostream>
#include <algorithm>
#include <unistd.h>

#include "localcontroller.h"
#include "tone.h"
#include "midi.h"

bool done = false;

int main() {
  LocalController lc;
  MidiStream ms;
  ms.Init();

  FreqList old_peaks;
  
  while (!done) {
    Tone tone;
    lc.GetData(tone.interval, tone.raw_audio);

    FreqList peaks = tone.GetPeakPitches();

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

    old_peaks = peaks;
  }
}

