#include <iostream>

#include "input.h"
#include "localcontroller.h"
#include "tone.h"

bool done = false;
KeyEventSender kes;

int main() {
  LocalController lc;
  int oldkey = -1;
  double oldvolume = -100.0;

  try {
    kes.Connect();
    while (!done) {
      Tone tone(lc.GetData());
      int newkey;
      int peakfreq = tone.GetPeakFrequency();
      int newvolume = tone.GetPitchStrength(peakfreq);
      if (peakfreq >= 160 and peakfreq < 175) {
	newkey = KEY_F1;
      } else if (peakfreq >= 175 and peakfreq < 180) {
	newkey = KEY_F2;
      } else if (peakfreq >= 180 and peakfreq < 190) {
	newkey = KEY_F3;
      } else if (peakfreq >= 190 and peakfreq < 200) {
	newkey = KEY_F4;
      } else if (peakfreq >= 200 and peakfreq < 225) {
	newkey = KEY_F5;
      } else {
	newkey = -1;
      }
      if (newkey != oldkey) {
	if (oldkey != -1) {
	  kes.Buffer(oldkey, 0);
	}
	if (newkey != -1) {
	  kes.Buffer(newkey, 1);
	  kes.Buffer(KEY_ENTER, 1);
	}
      } else {
	if (newvolume > oldvolume + 1.0 and newvolume > 1.0) {
	  kes.Buffer(KEY_ENTER, 1);
	} else {
	  kes.Buffer(KEY_ENTER, 0);
	}
      }
      kes.Send();
      oldkey = newkey;
      oldvolume = newvolume;
    }
    kes.Disconnect();
  } catch (const char *err) {
    std::cerr << err << std::endl;
  }
}

