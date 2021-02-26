#include "tone.h"

Tone::Tone() {}

Tone::Tone(double *interval) {
    this->interval = interval;
    this->threshold = 3.0; // Maybe, IDK whabt a good default for this is yet
}

Tone::Tone(double *interval, double threshold) {
    this->interval = interval;
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

int Tone::GetPeakFrequency() {
    double max = 0.0;
    int idx = 0;
    for (int i = 0; i < fft_size / 2 + 1; i++) {
	if (interval[i] > max) {
	    max = interval[i];
	    idx = i;
	}
    }
    return idx * max_hz * 2 / fft_size;
}

void Tone::SetFFTSize(int fft_size) {
    this->fft_size = fft_size;
}

void Tone::SetMaxHz(int max_hz) {
    this->max_hz = max_hz;
}
    
Tone::~Tone() {
    if (interval) {
	delete[] interval;
    }
}

void Tone::SetThreshold(double threshold) {
    this->threshold = threshold;
}

double Tone::GetThreshold() {
    return threshold;
}
