#include "tone.h"


Tone::Tone() : Tone (2048, 10025, 20.0) {}

Tone::Tone(double threshold) : Tone(1024, 10025, threshold) {}

Tone::Tone(int fft_size, int max_hz) : Tone(fft_size, max_hz, 20.0) {}

Tone::Tone(int fft_size, int max_hz, double threshold) {
  this->fft_size = fft_size;
  this->max_hz = max_hz;
  this->threshold = threshold;

  interval = new double[fft_size];
  raw_audio = new float[fft_size];
}

Tone::~Tone() {
  delete [] interval;
  delete [] raw_audio;
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

FreqList Tone::GetPeakPitches() {
  FreqList peaks;
  for (int i = 1; i < fft_size / 2 + 1; i++) {
    if (interval[i] < interval[i - 1]) {
      if (interval[i] > 1.0f) {
	peaks.push_back(i * max_hz / fft_size);
      }
    }
  }
  return peaks;
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
