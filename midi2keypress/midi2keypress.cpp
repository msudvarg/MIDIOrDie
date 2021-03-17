#include <iostream>
#include <algorithm>
#include <unistd.h>

#include "localcontroller.h"
#include "tone.h"
#include "midi.h"

bool done = false;

int main(int argc, char *argv[]) {
  int opt;
  bool all;
  
  while((opt = getopt(argc, argv, "a")) != -1) {
    switch(opt) {
    case 'a':
      all = true;
      break;
    }
  }
  
  LocalController lc;
  MidiStream ms;
  ms.Init();

  FreqList old_peaks;
  
  while (!done) {
    Tone tone;
    lc.GetData(tone.interval, tone.raw_audio);

    FreqList peaks = tone.GetPeakPitches();

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
}

