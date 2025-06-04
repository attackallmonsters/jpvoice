// TriangleOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>

// Triangle oscillator generates triangle waveform
class TriangleOscillator : public Oscillator
{
public:
    // Default constructor
    TriangleOscillator();

private:
    // Next sample block generation
    static void setSamplesIntern(DSPObject *dsp);
};