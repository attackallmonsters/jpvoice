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
    // Generates the next audio sample
    static void getSampleIntern(DSPBase *dsp, double &left, double &right);
};