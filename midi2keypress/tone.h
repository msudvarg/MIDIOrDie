#ifndef TONE_H
#define TONE_H

#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstring>

#define OUTPUT_FFT_MAX_HZ 2000
#define OUTPUT_FFT_SIZE   80
#define WINDOW_SIZE       1760
#define DELTA_HZ          25
#define HARMONICS 8   // Default number of harmonics to capture in the signature

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
constexpr double NORMAL_HZ = OUTPUT_FFT_MAX_HZ / HARMONICS;   // The signature represents a FFT of a note at this frequency (in Hz)
                                                              // Its amplitude serves as a threshold that must be met for a note to be considered "playing"

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


  double interval[WINDOW_SIZE];
  float raw_audio[WINDOW_SIZE];
  double signature[OUTPUT_FFT_SIZE];
private:

  // Tries to find frequency in provided fft. Returns the gain detected compared to the signature amplitude.
  // Returns 0 if frequency not found
  // If remove is true, then fft is modified to remove the detected note.
  double GetFrequencyPower(int note, double* fft, bool remove=false);

  void InterpolateAlias(double* a, double* b, int lenA, int lenB);

  double NoteToFreq(int note);
  int FreqToNote(double freq, int round=0);

  double threshold;
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
