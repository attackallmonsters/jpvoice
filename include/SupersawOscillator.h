// SupersawOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>
#include "dsp_types.h"

// Internal voice struct used for detuned oscillators
struct SupersawVoice
{
    dsp_float phase;
    dsp_float detune_ratio;
    dsp_float amp_ratio;
    dsp_float gainL;
    dsp_float gainR;
};

constexpr int NUM_VOICES = 7;
constexpr dsp_float DETUNE_RATIOS[NUM_VOICES] = {-0.24, -0.14, -0.06, 0.00, 0.06, 0.14, 0.24};
constexpr dsp_float AMP_RATIOS[NUM_VOICES] = {0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25};

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
    void setDetune(dsp_float value) override;

private:
    // Sample calculation without looking up vtable
    static void generateSample(Oscillator *, const dsp_float &, dsp_float &, dsp_float &);
    
    SupersawVoice voices[NUM_VOICES]; // All detuned voices
    dsp_float detune = 0.0;              // Voices detune
    dsp_float norm = 0.9;                // Loudness normalization
};