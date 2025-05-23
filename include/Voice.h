#pragma once

#include "Oscillator.h"

// The PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// The Voice class manages two oscillators and handles interactions like
// Frequency Modulation (FM) and Oscillator Sync between them.
class Voice
{
public:
    // Constructor: requires two Oscillator pointers and the global sample rateSample
    Voice(Oscillator *o1, Oscillator *o2);

    // Destructor: deletes both oscillator instances
    ~Voice();

    // Osc1<>Osc2 mix: 0: Osc1 only; 1: Osc2 only
    void setMix(double mix);

    // Enables or disables frequency modulation (FM)
    void setFMEnabled(bool enabled);

    // Enables or disables oscillator sync
    void setSyncEnabled(bool enabled);

    // Sets the modulation index used when FM is active
    void setModIndex(double index);

    // Sets teh sample rate for signal calculation
    void setSampleRate(double sampleRate);

    // Sets the frequency of oscillator 1/carrier and oscillator 2/modulator
    void setFrequencies(double frequencyOsc1, double frequencyOsc2);

    // Computes and returns one audio sample by combining both oscillators at a given sample rate
    double getSample();

    // Applies changed paramters on low amplitude
    void applyChangedParameters();

private:
    Oscillator *osc1; // Carrier oscillator (may be modulated)
    Oscillator *osc2; // Modulator oscillator (for FM or sync)

    double oscmix = 0.5;          // Mix Osc1<>Osc2
    bool fmEnabled = false;       // True if FM is active
    bool syncEnabled = false;     // True if Sync is active
    double modulationIndex = 1.0; // FM depth: how much osc2 modulates osc1

    double detune = 0;        // Detune factor supersaw oszillator
    double pulseWidth = 0.5;  // Pulse width square oscillator

    bool paramChanged = false;    // Indicates a paramter change
    int fadeCounter = 0;          // Amplitude for param change
    const int fadeLength = 32;    // Fade in/out samples for param change
    double fadeValue = 1.0;       // Current amplitude for param change
};
