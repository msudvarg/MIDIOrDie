#include "common.h"
#include "tone.h"

Tone::Tone() {}

Tone::Tone(double *interval) {
    this->interval = interval;
    this->threshold = 0.5; // Maybe, IDK what a good default for this is yet
}

Tone::Tone(double *interval, double threshold) {
    this->interval = interval;
    this->threshold = threshold;
}

bool Tone::HasPitch(int frequency) {
    return GetPitchStrength(frequency) >= threshold;
}

double Tone::GetPitchStrength(int frequency) {
    int bucket_size = SAMPLE_RATE / ROLLING_WINDOW_SIZE;
    int index = (int) (frequency / bucket_size);
    if (index > ROLLING_WINDOW_SIZE) {
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
