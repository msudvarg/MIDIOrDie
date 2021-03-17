#ifndef TONE_H
#define TONE_H

#include <vector>
#include <algorithm>

#define OUTPUT_FFT_MAX_HZ 10025
#define OUTPUT_FFT_SIZE 1024

typedef std::vector<int> FreqList;

enum class Pitch {
  E2 = 82,
  F2 = 98,
  FS2 = 100,
  G2 = 98,
  GS2 = 103
};

class Tone {
public:
  Tone();
  Tone(double threshold);
  
  bool HasPitch(int frequency);
  double GetPitchStrength(int frequency);
  FreqList GetPeakPitches();
  int GetPeakPitch();
  float GetMaxWave();

  void SetThreshold(double threshold);
  double GetThreshold();

  void SetFFTSize(int fft_size);
  void SetMaxHz(int max_hz);

  double interval[OUTPUT_FFT_SIZE];
  float raw_audio[OUTPUT_FFT_SIZE];
private:
  double threshold;
  int max_hz = OUTPUT_FFT_MAX_HZ;
  int fft_size = OUTPUT_FFT_SIZE;
};

static inline FreqList FreqDifference(FreqList a, FreqList b) {
  FreqList v(20);
  auto it = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), v.begin());
  v.resize(it-v.begin());
  return v;
}

#endif /* TONE_H */
