#ifndef LOCALCONTROLLER_H
#define LOCALCONTROLLER_H

#include <fftw3.h>
#include <portaudio.h>
#include <mutex>

static constexpr int SAMPLE_RATE       = 44100;
static constexpr float WINDOW_LATENCY_MS = 40;
static constexpr int OUTPUT_FFT_MAX_HZ = 2000;  
static constexpr float DELTA_HZ        = 1000.0 / WINDOW_LATENCY_MS;  
static constexpr int WINDOW_SIZE = SAMPLE_RATE * WINDOW_LATENCY_MS / 1000;  // Number of samples that fit into latency window, rounded down to power of 2
static constexpr int OUTPUT_FFT_SIZE = OUTPUT_FFT_MAX_HZ / DELTA_HZ;        // Number of Hz bins to visualize

class LocalController {
public:
  LocalController();
  LocalController(int n, int fft_size, int sample_rate);
  void GetData(double *fft_data_out, float *raw_audio_out);
  void GetData(double *fft_data_out);
  double GetRefreshRate();
  ~LocalController();
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
};

#endif /* LOCALCONTROLLER_H */
