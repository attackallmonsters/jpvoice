// SupersawOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>

// Internal voice struct used for detuned oscillators
struct SupersawVoice
{
    double phase;
    double detune_ratio;
    double amp_ratio;
};

constexpr int NUM_VOICES = 7;
constexpr double DETUNE_RATIOS[NUM_VOICES] = {-0.24, -0.14, -0.06, 0.00, 0.06, 0.14, 0.24};
constexpr double AMP_RATIOS[NUM_VOICES] = {0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25};

// Supersaw oscillator generates a dense, chorused sawtooth waveform
// by summing multiple slightly detuned sawtooth waves.
class SupersawOscillator : public Oscillator
{
public:
    // Default constructor
    SupersawOscillator();

    // Resets the internal phase of all voices
    void resetPhase() override;

    // Sets the detune factor for the supersaw voices
    void setDetune(double value) override;

private:
    // Next sample block generation
    static void setSamplesIntern(DSP *dsp);
    
    SupersawVoice voices[NUM_VOICES]; // All detuned voices
    double detune = 0.0;              // Voices detune
    double norm = 0.9;                // Loudness normalization
};