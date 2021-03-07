#pragma once

#include <complex>
#include <cmath>
#include <string.h>
#include "shared_array.h"

static inline constexpr
uint_fast8_t log2floor (uint32_t value)
/* Computes the ceiling of log_2(value) */
{
    if (value >= 2)
    {
        uint32_t mask = 0x80000000;
        uint_fast8_t result = 31;
        value = value - 1;

        while (mask != 0) {
            if (value & mask)
                return result;
            mask >>= 1;
            --result;
        }
    }
    return 0;
}
static inline constexpr
uint_fast8_t log2ceil (uint32_t value)
/* Computes the ceiling of log_2(value) */
{
    if (value >= 2)
    {
        uint32_t mask = 0x80000000;
        uint_fast8_t result = 32;
        value = value - 1;

        while (mask != 0) {
            if (value & mask)
                return result;
            mask >>= 1;
            --result;
        }
    }
    return 0;
}

#define SAMPLE_RATE         44000
#define DELTA_HZ            5
#define WINDOW_LATENCY_MS   30  
#define OUTPUT_FFT_MAX_HZ   2000    
constexpr int SAMPLE_WINDOW_SIZE = SAMPLE_RATE * WINDOW_LATENCY_MS / 1000;  // Number of samples that fit into latency window, rounded down to power of 2
constexpr int ROLLING_WINDOW_SIZE = SAMPLE_RATE / DELTA_HZ;                 // Number of samples required to discern differences of delta_hz, rounded up to power of 2
constexpr int OUTPUT_FFT_SIZE = OUTPUT_FFT_MAX_HZ / DELTA_HZ;               // Number of Hz bins to visualize

struct Shared_Buffer {
    float max_hz;
    float bucket_hz;
    Shared_Array<double,OUTPUT_FFT_SIZE> fftData;
};

