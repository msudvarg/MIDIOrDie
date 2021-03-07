#include "tone.h"


Tone::Tone() {
    this->threshold = 20.0; // Maybe, IDK what a good default for this is yet
}

Tone::Tone(double threshold) {
    this->threshold = threshold;
}

bool Tone::HasPitch(int frequency) {
    return GetPitchStrength(frequency) >= threshold;
}

double Tone::GetPitchStrength(int frequency) {
    int index = (int) frequency * fft_size / max_hz;
    if (index > fft_size) {
	throw "Frequency is outside of sampling range";
    }
    return interval[index];
}

int Tone::GetPeakPitch() {
    double max = 0.0;
    int idx = 0;
    for (int i = 0; i < fft_size / 2 + 1; i++) {
	if (interval[i] > max) {
	    max = interval[i];
	    idx = i;
	}
    }
    return idx * max_hz / fft_size;
}

float Tone::GetMaxWave() {
  float max = 0.0;
  for (int i = 0; i < fft_size; i++) {
    if (raw_audio[i] > max) {
      max = raw_audio[i];
    }
  }
  return max;
}

void Tone::SetFFTSize(int fft_size) {
    this->fft_size = fft_size;
}

void Tone::SetMaxHz(int max_hz) {
    this->max_hz = max_hz;
}

void Tone::SetThreshold(double threshold) {
    this->threshold = threshold;
}

double Tone::GetThreshold() {
    return threshold;
}
