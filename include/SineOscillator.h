// SupersawOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>

// Supersaw oscillator generates a dense, chorused sawtooth waveform
// by summing multiple slightly detuned sawtooth waves.
class SineOscillator : public Oscillator
{
public:
    // Default constructor
    SineOscillator() = default;

    // Generates the next audio sample
    double getSample() override;    
};