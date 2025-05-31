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
    // Generates the next audio sample
    static void getSampleIntern(Oscillator *osc, double &left, double &right);
};