#include "tone.h"
#include <algorithm>
#include <iostream>


Tone::Tone() : Tone (FFT::WINDOW_SIZE, FFT::OUTPUT_FFT_MAX_HZ, 20.0) {}

Tone::Tone(double threshold) : Tone(FFT::WINDOW_SIZE, FFT::OUTPUT_FFT_MAX_HZ, threshold) {}

Tone::Tone(int fft_size, int max_hz) : Tone(fft_size, max_hz, 20.0) {}

Tone::Tone(int fft_size, int max_hz, double threshold) {
  this->fft_size = fft_size;
  this->max_hz = max_hz;
  this->threshold = threshold;

  //interval = new double[fft_size];
  raw_audio = new float[fft_size];     // NOTE: Dane, I dislike dynamically adjustable fft windows. Makes me nervous
}

Tone::~Tone() {
  //delete [] interval;
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
    return FreqToNote(idx * FFT::DELTA_HZ);
}

// Finds peaks in fft (defined as any bucket above 7th octile) and returns list of ints
// corresponding to notes. If more that one note resides in a bucket, it returns all of them
FreqList Tone::GetPeakPitches() {
  FreqList peaks;

  FFT::Shared_Array_t::array_type arr = interval;
  //std::vector<double> arr = std::vector<double>(interval, interval + FFT::OUTPUT_FFT_SIZE);
  std::sort(arr.begin(), arr.end());

  double threshold = arr[7*arr.size()/8];

  for(int i = 1; i < fft_size; i++) {
    if (interval[i] > threshold) {
      int bottom_note = FreqToNote(i * FFT::DELTA_HZ, 1);
      int top_note = FreqToNote((i+1) * FFT::DELTA_HZ, -1);

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

void Tone::InterpolateAlias(double* a, double* b, int lenA, int lenB) {
  double interpolation_factor = (double)(lenA - 1) / (lenB - 1);
  double dummy;

  for(int i = 0; i < lenB - 1; i++) {
    int lower_ind = (int)std::floor(i * interpolation_factor);
    double lower_weight = 1 - std::modf(i * interpolation_factor, &dummy);
    int upper_ind = (int)std::floor((i+1) * interpolation_factor);
    double upper_weight = std::modf((i+1) * interpolation_factor, &dummy);

    // Sum all a buckets that map to bucket b[i], making sure to alias the
    // edges and account for the fact these are decibels
    b[i] = (lower_weight * std::pow(10,a[lower_ind]/10) + upper_weight * std::pow(10,a[upper_ind]/10));
    for(int j = lower_ind + 1; j < upper_ind; j++) {
      b[i] += std::pow(10,a[j]/10);
    }
    b[i] = 10 * std::log10(b[i]);
  }

  b[lenB-1] = a[lenA-1];
}

void Tone::SetSignature(std::vector<double> sig) {
  Tone::SetSignature(sig.data(), sig.size());
}

/* Interpolates signature of arbitrary length into local storage for later use */
void Tone::SetSignature(double* sig, int length) {
  int fundamental_bin = FFT::OUTPUT_FFT_SIZE / 8;    // signature array needs to fit 8 harmonics,
                                                // so align fundamental frequency 8th of way along array
  InterpolateAlias(sig, signature, length, FFT::OUTPUT_FFT_SIZE);
}

void Tone::DummySignature() {
  for(int i = 0; i < FFT::OUTPUT_FFT_SIZE; i++) {
    signature[i] = -50;
  }

  signature[10] = 15;
  signature[20] = 12;
  signature[30] = 10.2;
  signature[40] = 9;
  signature[50] = 8;
  signature[60] = 0;
  signature[70] = -5;

  // signature[8] = 3;
  // signature[9] = 9;
  // signature[10] = 3;
  // signature[18] = 2;
  // signature[19] = 6;
  // signature[20] = 2;
  // signature[28] = 1;
  // signature[29] = 4.2;
  // signature[30] = 1;
  // signature[38] = 0;
  // signature[39] = 3;
  // signature[40] = 0;
  // signature[48] = -1;
  // signature[49] = 2;
  // signature[50] = -1;
  // signature[58] = -2;
  // signature[59] = 1.2;
  // signature[60] = -2;
  // signature[68] = -3;
  // signature[69] = 0.5;
  // signature[70] = -3;
  // signature[78] = -4;
  // signature[79] = 0;
  // signature[80] = -4;
}

FreqList Tone::ExtractSignatures() {
  return ExtractSignatures(GetPeakPitches());
}

// Primers contains a list of notes to look for
FreqList Tone::ExtractSignatures(FreqList primers) {
  FreqList found;
  double bucket_size = max_hz / fft_size;

  double fftcopy[FFT::OUTPUT_FFT_SIZE];
  memcpy(fftcopy, interval.data(), FFT::OUTPUT_FFT_SIZE * sizeof(double));

  for(int f : primers) {
    if (f < FreqToNote(FFT::OUTPUT_FFT_MAX_HZ)) {
      if (GetFrequencyPower(f, fftcopy, false) > 0) {
        found.push_back(f);
      }
    }
  }

  // TODO: Maybe compare gains of notes or something?

  return found;
  
}

double Tone::GetFrequencyPower(int note, double* fft, bool remove) {
  double hz = NoteToFreq(note);

  double scaling_factor = hz / NORMAL_HZ;
  int numBins = (int)(FFT::OUTPUT_FFT_SIZE * scaling_factor);
  if (numBins < 0) return 0;
  double* noteSig = (double*)malloc(numBins * sizeof(double));

  InterpolateAlias(signature, noteSig, FFT::OUTPUT_FFT_SIZE, numBins);

  double gain = INFINITY;
  int matches = 0;

  for(int i = 0; i < numBins && i < FFT::OUTPUT_FFT_SIZE; i++) {
    if (fft[i] > noteSig[i]) {
      matches++;
      if (fft[i] - noteSig[i] < gain) {
        gain = fft[i] - noteSig[i];
      }
    }
  }

  if (matches <
        MATCH_CONFIDENCE * ((numBins > FFT::OUTPUT_FFT_SIZE) ? FFT::OUTPUT_FFT_SIZE : numBins)) {
    free(noteSig);
    return 0;
  }

  if (remove) {
    for(int i = 0; i < numBins && i < FFT::OUTPUT_FFT_SIZE; i++) {
      fft[i] -= gain*noteSig[i];
    }
  }

  free(noteSig);
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
    
    //Should never get here:
    default:
      return "";
  }
}

void Tone::PrintFFT() {
  for(int i = 0; i < 20; i++) {
    for(int j = -10; j < interval[i]; j++)
      std::cout << "*";
    std::cout << std::endl;
  }
  std::cout << "-----------------" << std::endl;
}
