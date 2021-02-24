#include "common.h"
#include "tone.h"

Tone::Tone() {}

Tone::Tone(double *interval) {
    this->interval = interval;
    this->threshold = 100.0; // Maybe, IDK what a good default for this is yet
}

Tone::Tone(double *interval, double threshold) {
    this->interval = interval;
    this->threshold = threshold;
}

bool Tone::HasPitch(int frequency) {
    return GetPitchStrength(frequency) >= threshold;
}

double Tone::GetPitchStrength(int frequency) {
    int bucket_size = OUTPUT_FFT_MAX_HZ / OUTPUT_FFT_SIZE;
    int index = (int) (frequency / bucket_size);
    if (index > OUTPUT_FFT_SIZE) {
	throw "Frequency is outside of sampling range";
    }
    return interval[index];
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
