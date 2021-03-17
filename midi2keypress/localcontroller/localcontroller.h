#ifndef LOCALCONTROLLER_H
#define LOCALCONTROLLER_H

#include <fftw3.h>
#include <portaudio.h>
#include <mutex>

#define DEFAULT_N 2048
#define DEFAULT_SAMPLE_RATE 10025

class LocalController {
public:
  LocalController();
  LocalController(int n, int sample_rate);
  void GetData(double *fft_data_out, float *raw_audio_out);
  double GetRefreshRate();
  ~LocalController();
private:
  int n, sample_rate;
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
};

#endif /* LOCALCONTROLLER_H */
