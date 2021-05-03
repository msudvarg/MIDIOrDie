#ifndef LOCALCONTROLLER_H
#define LOCALCONTROLLER_H

#include <fftw3.h>
#include <portaudio.h>
#include <mutex>
#include "../include/manifest.h"
#include "../include/timing.h"

class LocalController {
public:
  LocalController();
  LocalController(int n, int fft_size, int sample_rate);
  void GetData(float *fft_data_out, float *raw_audio_out);
  void GetData(float *fft_data_out);
  float GetRefreshRate();
  ~LocalController();
  TimingLog<Microseconds,200> fft_times;
private:
  int n, fft_size, sample_rate;
  float *window;
  float *fft_data;
  float *raw_audio;
  PaStream *stream;
  fftw_complex *in, *out;
  fftw_plan p;
  std::mutex mtx;
  static int callback (const void* input,
		       void *output,
		       unsigned long frameCount,
		       const PaStreamCallbackTimeInfo *timeInfo,
		       PaStreamCallbackFlags statusFlags,
		       void *userData);
  int callback (const void* input,
		       void *output,
		       unsigned long frameCount,
		       const PaStreamCallbackTimeInfo *timeInfo,
		       PaStreamCallbackFlags statusFlags);
};

#endif /* LOCALCONTROLLER_H */
