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
    TriangleOscillator() = default;

    // Generates the next audio sample
    void getSample(double &left, double &right) override;
};