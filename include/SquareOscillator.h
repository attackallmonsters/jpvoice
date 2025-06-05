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
    // Sample calculation without looking up vtable
    static void generateSample(Oscillator *, const double &, double &, double &);
    
    double dutyCycle = 0.5; // Duty cycle for square oscillator
};