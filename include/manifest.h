#pragma once

#include "shared_array.h"

//Socket manifest variables for default port/IP
constexpr char IPADDR[] = "0.0.0.0"; //Server binds to all available IPs
constexpr int PORTNO = 10520;

//FFT constants
constexpr int SAMPLE_RATE       = 44100;
constexpr float WINDOW_LATENCY_MS = 40;
constexpr int OUTPUT_FFT_MAX_HZ = 2000;  
constexpr float DELTA_HZ        = 1000.0 / WINDOW_LATENCY_MS;  
constexpr int WINDOW_SIZE = SAMPLE_RATE * WINDOW_LATENCY_MS / 1000;  // Number of samples that fit into latency window, rounded down to power of 2
constexpr int OUTPUT_FFT_SIZE = OUTPUT_FFT_MAX_HZ / DELTA_HZ;        // Number of Hz bins to visualize

//FFT 
using shared_fft_t = Shared_Array<float,OUTPUT_FFT_SIZE>;

//Clock type for poller and timing measurements
