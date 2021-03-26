
#include <unistd.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <iterator>

#include "../include/manifest.h"
#include "../include/poller.h"
#include "fft.h"

unsigned int FFT::bitReverse(unsigned int x, int log2n) {
    int n = 0;
    //int mask = 0x1; //Unused
    for (int i=0; i < log2n; i++) {
      n <<= 1;
      n |= (x & 1);
      x >>= 1;
    }
    return n;
}

inline constexpr
uint_fast8_t FFT::log2floor (uint32_t value)
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

inline constexpr
uint_fast8_t FFT::log2ceil (uint32_t value)
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

//Constructor
void FFT::init() {

    PaError err;
    
    std::cout << "Sample rate: " << SAMPLE_RATE << "Hz" << std::endl
        << "Precision: " << DELTA_HZ << "Hz" << std::endl
        << "Desired rolling window Latency: " << WINDOW_LATENCY_MS << "ms" << std::endl
        << "Samples per rolling window: " << WINDOW_SIZE << std::endl;

    err = Pa_Initialize();
    if (err != paNoError) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        throw err;
    }

    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                                1,          /* 1 input channel */
                                0,          /* no output */
                                paFloat32,  /* 32 bit floating point output */
                                SAMPLE_RATE,
                                WINDOW_SIZE,/* frames per buffer, i.e. the number
                                            of sample frames that PortAudio will
                                            request from the callback. Many apps
                                            may want to use
                                            paFramesPerBufferUnspecified, which
                                            tells PortAudio to pick the best,
                                            possibly changing, buffer size.*/
                                &FFT::paCallback, /* this is your callback function */
                                this ); /*This is a pointer that will be passed to
                                                   your callback*/
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        throw err;
    }

    /* Start stream */
    err = Pa_StartStream( stream );
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        throw err;
    }

    /* Sleep for a bit to collect data */
    Pa_Sleep(1000);

}

void FFT::run(bool forever) {

    PaError err;
    
    //FFT loop
    for(int i = 0; i < 1000 && !quit; forever ? i : i++) {
    
        Poller poller(polling_freq);

        while(!dataAvailable) {
            std::cout << "Looping..." << std::endl;
            usleep(5000);
        }

        // Copy latest sample into rolling window
        for(int i = 0; i < WINDOW_SIZE; i++) {
            complexInputBuffer[i] = std::complex<double>(sample_data[i], 0);
        }
        dataAvailable = false;

        // Perform fft
        fft(complexInputBuffer, complexOutputBuffer, log2(WINDOW_SIZE));
        getMag(complexOutputBuffer, finalOutputBuffer, WINDOW_SIZE);

        std::cout << "Performed fft" << std::endl;

        // Little bit of stats collection
        double max0=0, max1=0, max2=0;
        int max0_ind=0, max1_ind=0, max2_ind=0;
        for(int i = 0; i < WINDOW_SIZE/2; i++) {
            if(finalOutputBuffer[i] > max0) {
                max2 = max1;
                max2_ind = max1_ind;
                max1 = max0;
                max1_ind = max0_ind;
                max0 = finalOutputBuffer[i];
                max0_ind = i;
            } else if(finalOutputBuffer[i] > max1) {
                max2 = max1;
                max2_ind = max1_ind;
                max1 = finalOutputBuffer[i];
                max1_ind = i;
            } else if(finalOutputBuffer[i] > max2) {
                max2 = finalOutputBuffer[i];
                max2_ind = i;
            }
        }
        float bucket_size = (float)SAMPLE_RATE / WINDOW_SIZE;
        std::cout << "Largest frequency: " << bucket_size * (max0_ind+1) << "Hz" << std::endl
              << "2nd largest frequency: " << bucket_size * (max1_ind+1) << "Hz" << std::endl
              << "3rd largest frequency: " << bucket_size * (max2_ind+1) << "Hz" << std::endl << std::endl;


        //Copy FFT values to shared array
        sharedArray.write(finalOutputBuffer);
    }
    
    //Cleanup
    std::cout << "Number of callbacks: " << count << std::endl;

    /* Stop stream */
    err = Pa_StopStream(stream);
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        throw err;
    }

    /* Close program */
    err = Pa_CloseStream(stream);
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        throw err;
    }
    err = Pa_Terminate();
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        throw err;
    }

}

int FFT::paCallback(
        const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData )
{
    return reinterpret_cast<FFT *>(userData)->patestCallback(
        inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags
    );
}

int FFT::patestCallback(
        const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags )
{
    std::cout << "Callback called" << std::endl;
    if (framesPerBuffer < WINDOW_SIZE) return -1;

    count++;

    memcpy(sample_data, inputBuffer, framesPerBuffer * sizeof(float));

    dataAvailable = true;

    return 0;
}

template<class Iter_T>
void FFT::fft(Iter_T a, Iter_T b, int log2n)
{
    typedef typename std::iterator_traits<Iter_T>::value_type complex;
    const complex J(0, 1);
    unsigned int n = 1 << log2n;
    for (unsigned int i=0; i < n; ++i) {
        b[bitReverse(i, log2n)] = a[i];
    }
    for (int s = 1; s <= log2n; ++s) {
        unsigned int m = 1 << s;
        unsigned int m2 = m >> 1;
        complex w(1, 0);
        complex wm = exp(-J * (PI / m2));
        for (unsigned int j=0; j < m2; ++j) {
            for (unsigned int k=j; k < n; k += m) {
                complex t = w * b[k + m2];
                complex u = b[k];
                b[k] = u + t;
                b[k + m2] = u - t;
            }
            w *= wm;
        }
    }
}

template<class Iter_T>
double* FFT::getMag(Iter_T a, double* b, int length)
{

    for(int i = 0; i < length; i++) {
        b[i] = sqrt(a[i].real()*a[i].real() + a[i].imag()*a[i].imag());
    }

    return b;
}

