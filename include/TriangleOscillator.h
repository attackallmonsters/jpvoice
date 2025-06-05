// TriangleOscillator.h

#pragma once

#include "Oscillator.h"

// Triangle oscillator generates triangle waveform
class TriangleOscillator : public Oscillator
{
public:
    // Default constructor
    TriangleOscillator();

private:
    // Sample calculation without looking up vtable
    static void generateSample(Oscillator *, const double &, double &, double &);
};