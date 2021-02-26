#ifndef TONE_H
#define TONE_H

#define OUTPUT_FFT_MAX_HZ 10025
#define OUTPUT_FFT_SIZE 1024

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
  int GetPeakFrequency();

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

#endif /* TONE_H */
