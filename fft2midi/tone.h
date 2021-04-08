#ifndef TONE_H
#define TONE_H

#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstring>

#include "../include/manifest.h"

#define HARMONICS 8   // Default number of harmonics to capture in the signature
#define MATCH_CONFIDENCE  0.9

// 40 = E2
// 45 = A2
// 50 = D3
// 55 = G3
// 59 = B3
// 64 = E4
typedef std::vector<int> FreqList;

#define SEMITONE  1.059463094
#define C0_HZ     16.35
#define C0        12
constexpr float NORMAL_HZ = OUTPUT_FFT_MAX_HZ / HARMONICS;   // The signature represents a FFT of a note at this frequency (in Hz)
                                                              // Its amplitude serves as a threshold that must be met for a note to be considered "playing"

class Tone {
public:
/*
  Tone();
  Tone(float threshold);
  Tone(int fft_size, int max_hz);
  Tone(int fft_size, int max_hz, float threshold);
  ~Tone();
  */
  Tone() = default;
  ~Tone() = default;
  
  bool HasPitch(int frequency);
  float GetPitchStrength(int frequency);
  FreqList GetPeakPitches();
  FreqList Hillclimb();
  int GetPeakPitch();
  float GetMaxWave();
  
  void SetSignature(std::vector<float> sig);
  void SetSignature(float* sig, int length);
  void DummySignature();

  FreqList ExtractSignatures();
  FreqList ExtractSignatures(FreqList primers);

  void SetThreshold(float threshold);
  float GetThreshold();

  void SetFFTSize(int fft_size);
  void SetMaxHz(int max_hz);

  std::string GetNoteName(int note);

  void PrintFFT();

  shared_fft_t::array_type interval;
  shared_fft_t::array_type raw_audio;
  shared_fft_t::array_type signature;
  
  /*
  float* interval;
  float* raw_audio;
  float signature[OUTPUT_FFT_SIZE];
  */

private:

  // Tries to find frequency in provided fft. Returns the gain detected compared to the signature amplitude.
  // Returns 0 if frequency not found
  // If remove is true, then fft is modified to remove the detected note.
  float GetFrequencyPower(int note, float* fft, bool remove=false);

  void InterpolateAlias(float* a, float* b, int lenA, int lenB);

  float NoteToFreq(int note);
  int FreqToNote(float freq, int round=0);

  float threshold = 20.0;
  int harmonics_captured = HARMONICS;
  int max_hz = OUTPUT_FFT_MAX_HZ;
  int fft_size = OUTPUT_FFT_SIZE;
};

static inline FreqList FreqDifference(FreqList a, FreqList b) {
  FreqList v(a.size());
  auto it = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), v.begin());
  v.resize(it-v.begin());
  return v;
}

#endif /* TONE_H */
