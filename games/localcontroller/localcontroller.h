#ifndef LOCALCONTROLLER_H
#define LOCALCONTROLLER_H

#include <fftw3.h>
#include <portaudio.h>
#include <mutex>

#define N 1024
#define SAMPLE_RATE 11025

class LocalController {
public:
  LocalController();
  double *GetData();
  ~LocalController();
private:
  float data[N];
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
