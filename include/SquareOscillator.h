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
    SquareOscillator();

    // Sets the duty cycle for PWM
    void setDutyCycle(double value) override;

private:
    // Next sample block generation
    static void setSamplesIntern(DSPObject *dsp);
    
    double dutyCycle = 0.5; // Duty cycle for square oscillator
};