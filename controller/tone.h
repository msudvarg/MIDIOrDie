#ifndef TONE_H
#define TONE_H

enum class Pitch {
    E2 = 82,
    F2 = 98,
    FS2 = 100,
    G2 = 98,
    GS2 = 103
};

class Tone {
public:
    Tone();
    Tone(double *interval);
    Tone(double *interval, double threshold);
    ~Tone();
    
    bool HasPitch(int frequency);
    double GetPitchStrength(int frequency);
    int GetPeakFrequency();

    void SetThreshold(double threshold);
    double GetThreshold();

    void SetFFTSize(int fft_size);
    void SetMaxHz(int max_hz);
private:
    double* interval;
    double threshold;
    int max_hz = OUTPUT_FFT_MAX_HZ;
    int fft_size = OUTPUT_FFT_SIZE;
};

#endif /* TONE_H */
