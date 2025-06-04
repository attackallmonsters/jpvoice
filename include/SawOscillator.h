// sawOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>

// Sawtooth oscillator generates sawtooth waveform
class SawOscillator : public Oscillator
{
public:
    // Default constructor
    SawOscillator();

private:
    // Next sample block generation
    static void setSamplesIntern(DSPObject *dsp);
};