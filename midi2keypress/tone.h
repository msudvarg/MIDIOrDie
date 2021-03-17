#ifndef TONE_H
#define TONE_H

#include <vector>
#include <algorithm>

#define OUTPUT_FFT_MAX_HZ 10025
#define OUTPUT_FFT_SIZE 2048

typedef std::vector<int> FreqList;

class Tone {
public:
  Tone();
  Tone(double threshold);
  Tone(int fft_size, int max_hz);
  Tone(int fft_size, int max_hz, double threshold);
  ~Tone();
  
  bool HasPitch(int frequency);
  double GetPitchStrength(int frequency);
  FreqList GetPeakPitches();
  int GetPeakPitch();
  float GetMaxWave();

  void SetThreshold(double threshold);
  double GetThreshold();

  void SetFFTSize(int fft_size);
  void SetMaxHz(int max_hz);

  double *interval;
  float *raw_audio;
private:
  double threshold;
  int max_hz;
  int fft_size;
};

static inline FreqList FreqDifference(FreqList a, FreqList b) {
  FreqList v(a.size());
  auto it = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), v.begin());
  v.resize(it-v.begin());
  return v;
}

#endif /* TONE_H */
