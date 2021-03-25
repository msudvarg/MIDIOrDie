#include "tone.h"
#include <algorithm>


Tone::Tone() : Tone (WINDOW_SIZE, OUTPUT_FFT_MAX_HZ, 20.0) {}

Tone::Tone(double threshold) : Tone(WINDOW_SIZE, OUTPUT_FFT_MAX_HZ, threshold) {}

Tone::Tone(int fft_size, int max_hz) : Tone(fft_size, max_hz, 20.0) {}

Tone::Tone(int fft_size, int max_hz, double threshold) {
  this->fft_size = fft_size;
  this->max_hz = max_hz;
  this->threshold = threshold;

  interval = new double[fft_size];
  raw_audio = new float[fft_size];     // NOTE: Dane, I dislike dynamically adjustable fft windows. Makes me nervous
}

Tone::~Tone() {
  delete [] interval;
  delete [] raw_audio;
}

double Tone::NoteToFreq(int note) {
  return C0_HZ * std::pow(SEMITONE, note - C0);
}

int Tone::FreqToNote(double freq, int round) {
  double factor = freq / C0_HZ;

  if (round < 1) {
    return (int)std::floor(std::log(factor) / std::log(SEMITONE));
  } else if (round == 0) {
    return (int)std::round(std::log(factor) / std::log(SEMITONE));
  } else {
    return (int)std::ceil(std::log(factor) / std::log(SEMITONE));
  }
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
    for (int i = 0; i < fft_size; i++) {
      if (interval[i] > max) {
          max = interval[i];
          idx = i;
      }
    }
    return FreqToNote(idx * DELTA_HZ);
}

// Finds peaks in fft (defined as any bucket above 3rd quartile) and returns list of ints
// corresponding to notes. If more that one note resides in a bucket, it returns all of them
FreqList Tone::GetPeakPitches() {
  FreqList peaks;

  std::vector<double> arr = std::vector<double>(interval, interval + fft_size);
  std::sort(arr.begin(), arr.end());

  double threshold = arr.data()[3*arr.size()/4];

  for(int i = 1; i < fft_size; i++) {
    if (interval[i] > threshold) {
      int bottom_note = FreqToNote(i * DELTA_HZ, 1);
      int top_note = FreqToNote((i+1) * DELTA_HZ, -1);

      for(int j = bottom_note; j <= top_note; j++) {
        peaks.push_back(j);
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

// TODO: Fix this
void Tone::InterpolateAlias(double* a, double* b, int lenA, int lenB) {
  double interpolation_factor = (double)(lenA - 1) / (lenB - 1);

  for(int i = 0; i < lenB - 1; i++) {
    double j = i * interpolation_factor;
    int lower_ind = (int)std::floor(j);
    int upper_ind = (int)std::ceil(j);
    double weight = std::modf(j, new double);

    b[i] = weight * a[lower_ind] + (1-weight) * a[upper_ind];
  }

  b[lenB-1] = a[lenA-1];
}

void Tone::SetSignature(std::vector<double> sig) {
  Tone::SetSignature(sig.data(), sig.size());
}

/* Interpolates signature of arbitrary length into local storage for later use */
void Tone::SetSignature(double* sig, int length) {
  int fundamental_bin = OUTPUT_FFT_SIZE / 8;    // signature array needs to fit 8 harmonics,
                                                // so align fundamental frequency 8th of way along array
  InterpolateAlias(sig, signature, length, OUTPUT_FFT_SIZE);
}

void Tone::DummySignature() {
  for(int i = 0; i < OUTPUT_FFT_SIZE; i++) {
    signature[i] = -50;
  }

  signature[9] = -34;
  signature[19] = -37;
  signature[29] = -38.8;
  signature[39] = -40;
  signature[49] = -41;
  signature[59] = -41.8;
  signature[69] = -42.5;
  signature[79] = -43;
}

FreqList Tone::ExtractSignatures() {
  return ExtractSignatures(GetPeakPitches());
}

// Primers contains a list of notes to look for
FreqList Tone::ExtractSignatures(FreqList primers) {
  FreqList found;
  double bucket_size = max_hz / fft_size;

  double fftcopy[OUTPUT_FFT_SIZE];
  memcpy(fftcopy, interval, OUTPUT_FFT_SIZE);

  for(int f : primers) {
    if (GetFrequencyPower(f, fftcopy, true) > 0) {
      found.push_back(f);
    }
  }

  // TODO: Maybe compare gains of notes or something?

  return found;
  
}

double Tone::GetFrequencyPower(int note, double* fft, bool remove) {
  double hz = NoteToFreq(note);

  double scaling_factor = hz / NORMAL_HZ;
  int numBins = (int)(OUTPUT_FFT_SIZE * scaling_factor);
  if (numBins < 0) return 0;
  double* noteSig = (double*)malloc(numBins * sizeof(double));

  InterpolateAlias(signature, noteSig, OUTPUT_FFT_SIZE, numBins);

  double gain = INFINITY;
  int matches = 0;

  for(int i = 0; i < numBins && i < OUTPUT_FFT_SIZE; i++) {
    if (fft[i] > noteSig[i]) {
      matches++;
      if (fft[i] / noteSig[i] < gain) {
        gain = fft[i] / noteSig[i];
      }
    }
  }

  if (matches < (numBins > OUTPUT_FFT_SIZE) ? numBins : OUTPUT_FFT_SIZE) {
    return 0;
  }

  if (remove) {
    for(int i = 0; i < numBins && i < OUTPUT_FFT_SIZE; i++) {
      fft[i] -= gain*noteSig[i];
    }
  }

  return gain;
}

/* Highly reccomend not using */
void Tone::SetFFTSize(int fft_size) {
    this->fft_size = fft_size;
}

/* Highly reccomend not using */
void Tone::SetMaxHz(int max_hz) {
    this->max_hz = max_hz;
}

void Tone::SetThreshold(double threshold) {
    this->threshold = threshold;
}

double Tone::GetThreshold() {
    return threshold;
}

std::string Tone::GetNoteName(int note) {
  note += 9;
  int octave = note / 12;
  switch (note % 12) {
    case 0:
      return "C" + std::to_string(octave);
      break;
    case 1:
      return "C#" + std::to_string(octave);
      break;
    case 2:
      return "D" + std::to_string(octave);
      break;
    case 3:
      return "D#" + std::to_string(octave);
      break;
    case 4:
      return "E" + std::to_string(octave);
      break;
    case 5:
      return "F" + std::to_string(octave);
      break;
    case 6:
      return "F#" + std::to_string(octave);
      break;
    case 7:
      return "G" + std::to_string(octave);
      break;
    case 8:
      return "G#" + std::to_string(octave);
      break;
    case 9:
      return "A" + std::to_string(octave);
      break;
    case 10:
      return "A#" + std::to_string(octave);
      break;
    case 11:
      return "B" + std::to_string(octave);
      break;
  }
}
