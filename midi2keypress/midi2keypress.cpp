#include <iostream>

#define OUTPUT_FFT_MAX_HZ 10025
#define OUTPUT_FFT_SIZE 1024

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
      switch (peakfreq) {
      case 170:
	newkey = KEY_F1;
	break;
      case 180:
	newkey = KEY_F2;
	break;
      case 190:
	newkey = KEY_F3;
	break;
      case 200:
	newkey = KEY_F4;
	break;
      case 210:
	newkey = KEY_F5;
	break;
      default:
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

