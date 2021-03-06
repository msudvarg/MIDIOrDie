#pragma once

#include <complex>
#include <vector>
#include "portaudio.h"
#include "../include/shared_array.h" //Thread-safe array
#include "../include/manifest.h"

class FFT {

private:

    static unsigned int bitReverse(unsigned int x, int log2n);

    template<class Iter_T>
    void fft(Iter_T a, Iter_T b, int log2n);

    template<class Iter_T>
    double* getMag(Iter_T a, double* b, int length);

    template<class Iter_T>
    Iter_T hilbert_function(Iter_T a);

    // This takes the first derivative and returns the index of every rising edge event over the specified threshhold.
    // Used for finding attack events
    template<class Iter_T>
    std::vector<int> get_cliffs(Iter_T a, double threshhold);

    static int paCallback( const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData );

    int paCallback( const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags );

    /* Computes the ceiling of log_2(value) */
    static inline constexpr
    uint_fast8_t log2floor (uint32_t value);

    /* Computes the ceiling of log_2(value) */
    static inline constexpr
    uint_fast8_t log2ceil (uint32_t value);

    const double PI = 3.1415926536;
    float sample_data[WINDOW_SIZE];
    std::complex<double> complexInputBuffer[WINDOW_SIZE];
    std::complex<double> complexOutputBuffer[WINDOW_SIZE];
    double finalOutputBuffer[WINDOW_SIZE];
    bool dataAvailable = false;
    int count = 0;
    PaStream *stream;

    bool quit = false;

public:
    
    //Thread-safe array to send FFT data over socket
    using Shared_Array_t = Shared_Array<double,OUTPUT_FFT_SIZE>;

private:

    Shared_Array_t sharedArray;

public:

    void init();
    void run();
    void end();
    Shared_Array_t::array_type read() { return sharedArray.read(); }


    FFT() = default;
    ~FFT() noexcept = default;

};