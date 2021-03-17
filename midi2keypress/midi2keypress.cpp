#include <iostream>
#include <unistd.h>

#include "localcontroller.h"
#include "tone.h"
#include "midi.h"

bool done = false;

int main() {
  LocalController lc;
  int oldnote = -1;
  float lastmax = 0.0f;
  
  MidiStream ms;
  ms.Init();
  
  while (!done) {
    Tone tone;
    lc.GetData(tone.interval, tone.raw_audio);
    float maxamp = tone.GetMaxWave();
    int newnote;
    int peakfreq = tone.GetPeakPitch();
    std::cout << (int) Freq2Midi((float) peakfreq) << std::endl;
    if (peakfreq >= 160 and peakfreq < 175) {
      newnote = 52;
	} else if (peakfreq >= 175 and peakfreq < 180) {
      newnote = 53;
	} else if (peakfreq >= 180 and peakfreq < 190) {
      newnote = 54;
	} else if (peakfreq >= 190 and peakfreq < 200) {
      newnote = 55;
	} else if (peakfreq >= 200 and peakfreq < 225) {
      newnote = 56;
    } else {
      newnote = -1;
    }
    // Change the note
    if (newnote != oldnote) {
      if (oldnote != -1) {
	ms.Send(oldnote, false);
      }
      if (newnote != -1) {
	ms.Send(newnote, true);
      }
    }

    // Watch the attack
    if (maxamp > 0.0001 and maxamp > lastmax * 10 and newnote != -1) {
      // Attack again
      ms.Send(newnote, false);
      ms.Send(newnote, true);
    }

    lastmax = maxamp;
    oldnote = newnote;
  }
}

