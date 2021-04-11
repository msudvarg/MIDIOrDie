#ifndef TONE_H
#define TONE_H

#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstring>

#include "../include/manifest.h"
#include "../cppflow/include/cppflow/ops.h"
#include "../cppflow/include/cppflow/model.h"
#include "cnpy/cnpy.h"

#define HARMONICS 8   // Default number of harmonics to capture in the signature
#define MATCH_CONFIDENCE  0.9

// 40 = E2
// 45 = A2
// 50 = D3
// 55 = G3
// 59 = B3
// 64 = E4

#define SEMITONE  1.059463094
#define C0_HZ     16.35
#define C0        12
#define E2        40
constexpr float NORMAL_HZ = OUTPUT_FFT_MAX_HZ / HARMONICS;   // The signature represents a FFT of a note at this frequency (in Hz)
                                                              // Its amplitude serves as a threshold that must be met for a note to be considered "playing"
/*
class FreqList {
private:
  std::vector<int> list;
public:
  FreqList() {
    list.reserve(OUTPUT_FFT_SIZE/2);
  }
  using size_type = std::vector<int>::size_type;
  using iterator = std::vector<int>::iterator;
  using const_iterator = std::vector<int>::const_iterator;
  void push_back(int x) {list.push_back(x);}
  size_type size() const noexcept {return list.size();}
  int & operator[] (size_type i) { return list[i];}
  const int & operator[] (size_type i) const { return list[i];};
  iterator begin() noexcept {return list.begin();}
  const_iterator begin() const noexcept {return list.begin();}
  const_iterator cbegin() const noexcept {return list.cbegin();}
  iterator end() noexcept {return list.end();}
  const_iterator end() const noexcept {return list.end();}
  const_iterator cend() const noexcept {return list.cend();}

}; */
using FreqList = std::vector<int>;

class Tone {
public:
/*
  Tone();
  Tone(float threshold);
  Tone(int fft_size, int max_hz);
  Tone(int fft_size, int max_hz, float threshold);
  ~Tone();
  */
  Tone(std::string model_folder, std::string calibration_filename);
  ~Tone() = default;
  
  bool HasPitch(int frequency);
  float GetPitchStrength(int frequency);
  FreqList GetPeakPitches();
  FreqList Hillclimb();
  int GetPeakPitch();
  float GetMaxWave();
  
  template <class Container>
  void SetSignature(Container & sig);
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

  cppflow::model model;
  std::vector<float> calib;
  std::vector<float> silence;

  float threshold = 20.0;
  int harmonics_captured = HARMONICS;
  int max_hz = OUTPUT_FFT_MAX_HZ;
  int fft_size = OUTPUT_FFT_SIZE;
  int model_outputs = MODEL_OUTPUT_SIZE;
};

static inline FreqList FreqDifference(FreqList a, FreqList b) {
  FreqList v(a.size());
  auto it = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), v.begin());
  v.resize(it-v.begin());
  return v;
}

#endif /* TONE_H */
