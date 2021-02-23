#include <stdio.h>
#include <string.h>
#include <iostream>
#include <complex>
#include <cmath>
#include <iterator>
#include "portaudio.h"
#include "common.h"
#include "Shared_Memory.h"

using namespace std;

const double PI = 3.1415926536;
static float sample_data[SAMPLE_WINDOW_SIZE];
complex<double> complexInputBuffer[ROLLING_WINDOW_SIZE + SAMPLE_WINDOW_SIZE];
complex<double> complexOutputBuffer[ROLLING_WINDOW_SIZE];
double finalOutputBuffer[ROLLING_WINDOW_SIZE];
static bool dataAvailable = false;
static int count = 0;


unsigned int bitReverse(unsigned int x, int log2n) {
    int n = 0;
    int mask = 0x1;
    for (int i=0; i < log2n; i++) {
      n <<= 1;
      n |= (x & 1);
      x >>= 1;
    }
    return n;
}

template<class Iter_T>
void fft(Iter_T a, Iter_T b, int log2n)
{
    typedef typename iterator_traits<Iter_T>::value_type complex;
    const complex J(0, 1);
    int n = 1 << log2n;
    for (unsigned int i=0; i < n; ++i) {
        b[bitReverse(i, log2n)] = a[i];
    }
    for (int s = 1; s <= log2n; ++s) {
        int m = 1 << s;
        int m2 = m >> 1;
        complex w(1, 0);
        complex wm = exp(-J * (PI / m2));
        for (int j=0; j < m2; ++j) {
            for (int k=j; k < n; k += m) {
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
double* getMag(Iter_T a, double* b, int length)
{
    typedef typename iterator_traits<Iter_T>::value_type complex;

    for(int i = 0; i < length; i++) {
        b[i] = sqrt(a[i].real()*a[i].real() + a[i].imag()*a[i].imag());
    }

    return b;
}

static int patestCallback( const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData )
{
    if (framesPerBuffer < SAMPLE_WINDOW_SIZE) return -1;

    count++;

    memcpy(userData, inputBuffer, framesPerBuffer);

    dataAvailable = true;

    return 0;
}

int main(int argc, char** argv) {

    int opt;
    bool forever = false;

    while((opt = getopt(argc, argv, "f")) != -1) {
	switch(opt) {
	case 'f':
	    forever = true;
	    break;
	default:
	    std::cerr << "Usage: " << argv[0] << " [-f]\n" << std::endl;
	}
    }
    
    //Create shared memory for visualization
    Shared_Memory<Shared_Buffer> sharedBuffer {"fftData"}; 
    std::cout << "Shared memory size: " << sizeof(Shared_Buffer) << std::endl;
    sharedBuffer->lock_sequence = 0;
    memset(sharedBuffer->fftData, 0, sizeof(sharedBuffer->fftData));     // TODO: Pass max_hz and bucket_hz as well

    std::cout << "Sample rate: " << SAMPLE_RATE << "Hz" << std::endl
        << "Precision: " << DELTA_HZ << "Hz" << std::endl
        << "Desired rolling window Latency: " << WINDOW_LATENCY_MS << "ms" << std::endl
        << "Samples per rolling window: " << SAMPLE_WINDOW_SIZE << std::endl
        << "Actual rolling window latency: " << SAMPLE_WINDOW_SIZE * 1000 / SAMPLE_RATE << "ms" << std::endl
        << "Samples per full window: " << ROLLING_WINDOW_SIZE << std::endl
        << "Full window latency: " << ROLLING_WINDOW_SIZE * 1000 / SAMPLE_RATE << "ms" << std::endl;

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        return err;
    }

    PaStream *stream;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                                1,          /* 1 input channel */
                                0,          /* no output */
                                paFloat32,  /* 32 bit floating point output */
                                SAMPLE_RATE,
                                SAMPLE_WINDOW_SIZE,/* frames per buffer, i.e. the number
                                            of sample frames that PortAudio will
                                            request from the callback. Many apps
                                            may want to use
                                            paFramesPerBufferUnspecified, which
                                            tells PortAudio to pick the best,
                                            possibly changing, buffer size.*/
                                patestCallback, /* this is your callback function */
                                &sample_data ); /*This is a pointer that will be passed to
                                                   your callback*/
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        return err;
    }

    /* Start stream */
    err = Pa_StartStream( stream );
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        return err;
    }

    /* Sleep for a bit to collect data */
    Pa_Sleep(1000);

    // TODO: Program here (What program?)
    for(int i = 0; i < 1000; forever ? i : i++) {
        while(!dataAvailable);

        // Copy latest sample into rolling window
        for(int i = 0; i < SAMPLE_WINDOW_SIZE; i++) {
            complexInputBuffer[ROLLING_WINDOW_SIZE+i] = complex<double>(sample_data[i], 0);
        }
        dataAvailable = false;

        // Shift new sample into operable part of window
        for(int i = 0; i < ROLLING_WINDOW_SIZE; i++) {
            complexInputBuffer[i] = complexInputBuffer[i + SAMPLE_WINDOW_SIZE];
        }

        // Perform fft
        fft(complexInputBuffer, complexOutputBuffer, log2(ROLLING_WINDOW_SIZE));
        getMag(complexOutputBuffer, finalOutputBuffer, ROLLING_WINDOW_SIZE);

        // Little bit of stats collection
        double max0=0, max1=0, max2=0;
        int max0_ind=0, max1_ind=0, max2_ind=0;
        for(int i = 0; i < ROLLING_WINDOW_SIZE/2; i++) {
            if(finalOutputBuffer[i + ROLLING_WINDOW_SIZE/2] > max0) {
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
        int bucket_size = SAMPLE_RATE / ROLLING_WINDOW_SIZE;
        std::cout << "Largest frequency: " << bucket_size * (max0_ind+1) << "Hz" << std::endl
              << "2nd largest frequency: " << bucket_size * (max1_ind+1) << "Hz" << std::endl
              << "3rd largest frequency: " << bucket_size * (max2_ind+1) << "Hz" << std::endl << std::endl;

        sharedBuffer->lock_sequence++;
        memcpy(sharedBuffer->fftData, finalOutputBuffer, sizeof(sharedBuffer->fftData));        // Copy half of fft data into shared memory
        sharedBuffer->lock_sequence++;
    }
    std::cout << "Number of callbacks: " << count << std::endl;

    /* Stop stream */
    err = Pa_StopStream(stream);
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        return err;
    }

    /* Close program */
    err = Pa_CloseStream(stream);
    if( err != paNoError ) {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        return err;
    }
    err = Pa_Terminate();
    if( err != paNoError )
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        return err;

    return 0;    
}
