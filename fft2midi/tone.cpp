#include "tone.h"
#include <algorithm>
#include <iostream>

/*
Tone::Tone() : Tone (WINDOW_SIZE, OUTPUT_FFT_MAX_HZ, 20.0) {}

Tone::Tone(float threshold) : Tone(WINDOW_SIZE, OUTPUT_FFT_MAX_HZ, threshold) {}

Tone::Tone(int fft_size, int max_hz) : Tone(fft_size, max_hz, 20.0) {}

Tone::Tone(int _fft_size, int _max_hz, float _threshold) :
  fft_size(_fft_size),
  max_hz(_max_hz),
  threshold(_threshold)
{
  this->fft_size = fft_size;
  this->max_hz = max_hz;
  this->threshold = threshold;

  interval = new float[fft_size];
  raw_audio = new float[fft_size];     // NOTE: Dane, I dislike dynamically adjustable fft windows. Makes me nervous
                                       // OREN: People's audio interfaces are different, might need it for a card that
				       // has a different sample rate.
}
*/

/*
Tone::~Tone() {
  delete [] interval;
  delete [] raw_audio;
}
*/

float Tone::NoteToFreq(int note) {
  return C0_HZ * std::pow(SEMITONE, note - C0);
}

int Tone::FreqToNote(float freq, int round) {
  float factor = freq / C0_HZ;

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

float Tone::GetPitchStrength(int frequency) {
    int index = (int) frequency * fft_size / max_hz;
    return interval.at(index);
}

int Tone::GetPeakPitch() {
    float max = 0.0;
    int idx = 0;
    for (int i = 0; i < fft_size; i++) {
      if (interval.at(i) > max) {
          max = interval.at(i);
          idx = i;
      }
    }
    return FreqToNote(idx * DELTA_HZ);
}

// Finds peaks in fft (defined as any bucket above 7th octile) and returns list of ints
// corresponding to notes. If more that one note resides in a bucket, it returns all of them
FreqList Tone::GetPeakPitches() {
  FreqList peaks;

  shared_fft_t::array_type arr = interval;
  std::sort(arr.begin(), arr.end());

  float threshold = arr[7*arr.size()/8];
  

  for(int i = 1; i < fft_size; i++) {
    if (interval.at(i) > threshold) {
      int bottom_note = FreqToNote(i * DELTA_HZ, 1);
      int top_note = FreqToNote((i+1) * DELTA_HZ, -1);

      for(int j = bottom_note; j <= top_note; j++) {
        peaks.push_back(j);
      }
    }
  }
  return peaks;
}

FreqList Tone::Hillclimb() {
  FreqList peaks;
  
  for(int i = 1; i < fft_size; i++) {
    if (interval.at(i) < interval.at(i - 1)) {
      if (interval.at(i-1) > 1.0f) {
        peaks.push_back((i-1) * max_hz / fft_size);
      }
    }
  }
  
  return peaks;
}

FreqList Tone::HillClimbConstrained() {

  FreqList peaks;

  int max = 0;
  int max_index = 0;
  
  //Get largest peak
  for(int i = 1; i < fft_size; i++) {
    if(interval.at(i) > max) {
      max = interval.at(i);
      max_index = i;
    }
  }

  //Adjust if max in top half of frequency range
  if(max_index > fft_size/2) max_index = fft_size/2;

  //Iterate over 1 octave in either direction
  for(int i = max_index/2 + 1; i < max_index*2; ++i) {
    if (interval.at(i) < interval.at(i - 1)) {
      if (interval.at(i-1) > 1.0f) {
        peaks.push_back((i-1) * max_hz / fft_size);
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

void Tone::InterpolateAlias(float* a, float* b, int lenA, int lenB) {
  float interpolation_factor = (float)(lenA - 1) / (lenB - 1);
  float dummy;

  for(int i = 0; i < lenB - 1; i++) {
    int lower_ind = (int)std::floor(i * interpolation_factor);
    float lower_weight = 1 - std::modf(i * interpolation_factor, &dummy);
    int upper_ind = (int)std::floor((i+1) * interpolation_factor);
    float upper_weight = std::modf((i+1) * interpolation_factor, &dummy);

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

template <class Container>
void Tone::SetSignature(Container & sig) {
  Tone::SetSignature(sig.data(), sig.size());
}

/* Interpolates signature of arbitrary length into local storage for later use */
void Tone::SetSignature(float* sig, int length) {
  int fundamental_bin = OUTPUT_FFT_SIZE / 8;    // signature array needs to fit 8 harmonics,
                                                // so align fundamental frequency 8th of way along array
  InterpolateAlias(sig, signature.data(), length, signature.size());
}

void Tone::DummySignature() {
  for(int i = 0; i < OUTPUT_FFT_SIZE; i++) {
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
  float bucket_size = max_hz / fft_size;

  float fftcopy[OUTPUT_FFT_SIZE];
  memcpy(fftcopy, interval.data(), OUTPUT_FFT_SIZE * sizeof(float));

  for(int f : primers) {
    if (f < FreqToNote(OUTPUT_FFT_MAX_HZ)) {
      if (GetFrequencyPower(f, fftcopy, false) > 0) {
        found.push_back(f);
      }
    }
  }

  // TODO: Maybe compare gains of notes or something?

  return found;
  
}

float Tone::GetFrequencyPower(int note, float* fft, bool remove) {
  float hz = NoteToFreq(note);

  float scaling_factor = hz / NORMAL_HZ;
  int numBins = (int)(OUTPUT_FFT_SIZE * scaling_factor);
  if (numBins < 0) return 0;
  float* noteSig = (float*)malloc(numBins * sizeof(float));

  InterpolateAlias(signature.data(), noteSig, signature.size(), numBins);

  float gain = INFINITY;
  int matches = 0;

  for(int i = 0; i < numBins && i < OUTPUT_FFT_SIZE; i++) {
    if (fft[i] > noteSig[i]) {
      matches++;
      if (fft[i] - noteSig[i] < gain) {
        gain = fft[i] - noteSig[i];
      }
    }
  }

  if (matches <
        MATCH_CONFIDENCE * ((numBins > OUTPUT_FFT_SIZE) ? OUTPUT_FFT_SIZE : numBins)) {
    free(noteSig);
    return 0;
  }

  if (remove) {
    for(int i = 0; i < numBins && i < OUTPUT_FFT_SIZE; i++) {
      fft[i] -= gain*noteSig[i];
    }
  }

  free(noteSig);
  return gain;
}

void Tone::SetFFTSize(int fft_size) {
    this->fft_size = fft_size;
}

void Tone::SetMaxHz(int max_hz) {
    this->max_hz = max_hz;
}

void Tone::SetThreshold(float threshold) {
    this->threshold = threshold;
}

float Tone::GetThreshold() {
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

void Tone::PrintFFT() {
  for(int i = 0; i < 20; i++) {
    for(int j = -10; j < interval.at(i); j++)
      std::cout << "*";
    std::cout << std::endl;
  }
  std::cout << "-----------------" << std::endl;
}
