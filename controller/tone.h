#ifndef TONE_H
#define TONE_H

enum class Pitch {
    E2 = 92,
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

    void SetThreshold(double threshold);
    double GetThreshold();
private:
    double* interval;
    double threshold;
};

#endif /* TONE_H */
