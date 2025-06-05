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
    // Sample calculation without looking up vtable
    static void generateSample(Oscillator *, const double &, double &, double &);
};