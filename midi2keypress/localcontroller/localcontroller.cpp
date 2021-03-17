#include <string.h>
#include <math.h>

#include "localcontroller.h"

void hamming(int length, float *buffer) {
  for(int i = 0; i < length; i++) {
    buffer[i] = 0.54 - (0.46 * cosf(2 * M_PI * (i / ((length - 1) * 1.0))));
  }
}

LocalController::LocalController() : LocalController(DEFAULT_N, DEFAULT_SAMPLE_RATE) {}

LocalController::LocalController(int n, int sample_rate) {
  this->n = n;
  this->sample_rate = sample_rate;

  window = new float[n];
  fft_data = new float[n];
  raw_audio = new float[n];
  
  hamming(n, window);
  
  in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
  p = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_MEASURE);
  
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    throw Pa_GetErrorText(err);
  }
  
  err = Pa_OpenDefaultStream(
    &stream,
    1,
    1,
    paFloat32,
    sample_rate,
    n,
    callback,
    this);
  if (err != paNoError) {
    throw Pa_GetErrorText(err);
  }
  Pa_StartStream(stream);
}

int LocalController::callback (const void* input,
			       void *output,
			       unsigned long frameCount,
			       const PaStreamCallbackTimeInfo *timeInfo,
			       PaStreamCallbackFlags statusFlags,
			       void *userData) {
  LocalController *controller = (LocalController *) userData;
  float *inputBuffer = (float *) input;
  float *outputBuffer = (float *) output;
  memcpy(outputBuffer, inputBuffer, controller->n * sizeof(float));
  controller->mtx.lock();
  memcpy(controller->raw_audio, inputBuffer, controller->n * sizeof(float));
  for (int i = 0; i < controller->n; i++) {
    controller->in[i][0] = (double)inputBuffer[i] * controller->window[i];
    controller->in[i][1] = 0.0;
  }
  fftw_execute(controller->p);
  for (int i = 0; i < controller->n; i++) {
    controller->fft_data[i] = (float) sqrt(controller->out[i][0] * controller->out[i][0] + controller->out[i][1] * controller->out[i][1]);
    controller->fft_data[i] = 20 * log10f(controller->fft_data[i]);
  }
  controller->mtx.unlock();
  return 0;
}

void LocalController::GetData(double *fft_data_out, float *raw_audio_out) {
  mtx.lock();
  for (int i = 0; i < n; i++) {
    fft_data_out[i] = (double) fft_data[i];
  }
  memcpy(raw_audio_out, raw_audio, n * sizeof(float));
  mtx.unlock();
}

double LocalController::GetRefreshRate() {
  return 1.0 / Pa_GetStreamInfo(stream)->sampleRate / 1000.0;
}

LocalController::~LocalController() {
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
}
