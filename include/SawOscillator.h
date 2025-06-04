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
    // Sample calculation without looking up vtable
    static void computeSampleFuncIntern(Oscillator *, const double &, double &, double &);
};