#include <string.h>
#include <math.h>

#include "localcontroller.h"

void hamming(int length, float *buffer) {
  for(int i = 0; i < length; i++) {
    buffer[i] = 0.54 - (0.46 * cosf(2 * M_PI * (i / ((length - 1) * 1.0))));
  }
}

LocalController::LocalController() : LocalController(WINDOW_SIZE, OUTPUT_FFT_SIZE, SAMPLE_RATE) {}

LocalController::LocalController(int _n, int _fft_size, int _sample_rate) :
  n(_n),
  fft_size(_fft_size),
  sample_rate(_sample_rate),
  fft_times(TimingLogType::StartStop)
{

  window = new float[n];
  fft_data = new float[fft_size];
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
			       PaStreamCallbackFlags statusFlags) {
  fft_times.log();
  float *inputBuffer = (float *) input;
  float *outputBuffer = (float *) output;
  memcpy(outputBuffer, inputBuffer, n * sizeof(float));
  mtx.lock();
  memcpy(raw_audio, inputBuffer, n * sizeof(float));
  for (int i = 0; i < n; i++) {
    in[i][0] = (float)inputBuffer[i] * window[i];
    in[i][1] = 0.0;
  }
  fftw_execute(p);
  for (int i = 0; i < fft_size; i++) {
    fft_data[i] = (float) sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
    fft_data[i] = 20 * log10f(fft_data[i]);
  }
  mtx.unlock();
  fft_times.log();
  return 0;

}

int LocalController::callback (const void* input,
			       void *output,
			       unsigned long frameCount,
			       const PaStreamCallbackTimeInfo *timeInfo,
			       PaStreamCallbackFlags statusFlags,
			       void *userData) {
               
    return reinterpret_cast<LocalController *>(userData)->callback(
        input, output, frameCount, timeInfo, statusFlags
    );
}

void LocalController::GetData(float *fft_data_out, float *raw_audio_out) {
  mtx.lock();
  memcpy(fft_data_out, fft_data, fft_size * sizeof(float));
  memcpy(raw_audio_out, raw_audio, n * sizeof(float));
  mtx.unlock();
}

void LocalController::GetData(float *fft_data_out) {
  mtx.lock();
  memcpy(fft_data_out, fft_data, fft_size * sizeof(float));
  mtx.unlock();
}

float LocalController::GetRefreshRate() {
  return 1.0 / Pa_GetStreamInfo(stream)->sampleRate / 1000.0;
}

LocalController::~LocalController() {
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
  delete[] window;
  delete[] fft_data;
  delete[] raw_audio;
}
