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
    int peakfreq = tone.GetPeakPitch();
    
    if (peakfreq > 0) {
      int newnote = Freq2Midi(peakfreq);

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
}

