// SquareOscillator.h

#pragma once

#include "Oscillator.h"
#include <vector>
#include <cmath>

// Squaretooth oscillator generates square waveform
class SquareOscillator : public Oscillator
{
public:
    // Default constructor
    SquareOscillator() = default;

    // Sets the duty cycle for PWM
    virtual void setDutyCycle(double value) override;

    // Generates the next audio sample
    void getSample(double &left, double &right) override;

private:
    double dutyCycle = 0.5;
};