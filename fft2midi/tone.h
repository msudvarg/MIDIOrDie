#ifndef TONE_H
#define TONE_H

#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstring>

#include "../fft/fft.h"

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
constexpr double NORMAL_HZ = FFT::OUTPUT_FFT_MAX_HZ / HARMONICS;   // The signature represents a FFT of a note at this frequency (in Hz)
                                                              // Its amplitude serves as a threshold that must be met for a note to be considered "playing"

class Tone {
public:
/*
  Tone();
  Tone(double threshold);
  Tone(int fft_size, int max_hz);
  Tone(int fft_size, int max_hz, double threshold);
  ~Tone();
  */
  Tone() = default;
  ~Tone() = default;
  
  bool HasPitch(int frequency);
  double GetPitchStrength(int frequency);
  FreqList GetPeakPitches();
  int GetPeakPitch();
  float GetMaxWave();
  
  void SetSignature(std::vector<double> sig);
  void SetSignature(double* sig, int length);
  void DummySignature();

  FreqList ExtractSignatures();
  FreqList ExtractSignatures(FreqList primers);

  void SetThreshold(double threshold);
  double GetThreshold();

  void SetFFTSize(int fft_size);
  void SetMaxHz(int max_hz);

  std::string GetNoteName(int note);

  void PrintFFT();

  FFT::Shared_Array_t::array_type interval;
  std::array<float,FFT::OUTPUT_FFT_SIZE> raw_audio;
  std::array<double,FFT::OUTPUT_FFT_SIZE> signature;
  
  /*
  double* interval;
  float* raw_audio;
  double signature[FFT::OUTPUT_FFT_SIZE];
  */

private:

  // Tries to find frequency in provided fft. Returns the gain detected compared to the signature amplitude.
  // Returns 0 if frequency not found
  // If remove is true, then fft is modified to remove the detected note.
  double GetFrequencyPower(int note, double* fft, bool remove=false);

  void InterpolateAlias(double* a, double* b, int lenA, int lenB);

  double NoteToFreq(int note);
  int FreqToNote(double freq, int round=0);

  double threshold = 20.0;
  int harmonics_captured = HARMONICS;
  int max_hz = FFT::OUTPUT_FFT_MAX_HZ;
  int fft_size = FFT::OUTPUT_FFT_SIZE;
};

static inline FreqList FreqDifference(FreqList a, FreqList b) {
  FreqList v(a.size());
  auto it = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), v.begin());
  v.resize(it-v.begin());
  return v;
}

#endif /* TONE_H */
