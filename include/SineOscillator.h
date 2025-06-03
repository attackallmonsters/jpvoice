// SineOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>

// Sine oscillator generates sine waveform
class SineOscillator : public Oscillator
{
public:
    // Default constructor
    SineOscillator();

private:
    // Next sample block generation
    static void setSamplesIntern(DSP *dsp);
};