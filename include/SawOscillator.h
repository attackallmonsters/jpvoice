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
    SawOscillator() = default;

    // Generates the next audio sample
    void getSample(double &left, double &right) override;
};