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
    // Generates the next audio sample
    static void getSampleIntern(DSPBase *dsp, double &left, double &right);
    
    double dutyCycle = 0.5; // Duty cycle for square oscillator
};