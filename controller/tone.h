#ifndef TONE_H
#define TONE_H

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
