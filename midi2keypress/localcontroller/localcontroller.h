#ifndef LOCALCONTROLLER_H
#define LOCALCONTROLLER_H

#include <fftw3.h>
#include <portaudio.h>
#include <mutex>

#define N 2048
#define SAMPLE_RATE 10025

class LocalController {
public:
  LocalController();
  void GetData(double *fft_data_out, float *raw_audio_out);
  ~LocalController();
private:
  float window[N];
  float fft_data[N];
  float raw_audio[N];
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
