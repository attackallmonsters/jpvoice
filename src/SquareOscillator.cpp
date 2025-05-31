#include "SquareOscillator.h"
#include "clamp.h"

SquareOscillator::SquareOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SquareOscillator::getSampleIntern;
}

void SquareOscillator::getSampleIntern(Oscillator *osc, double &left, double &right)
{
    SquareOscillator *self = static_cast<SquareOscillator *>(osc);

    // Compute phase increment and update phase
    self->currentPhase += self->phaseIncrement;
    self->wrapped = false;

    // Wrap phase to stay within [0.0, 1.0) – works in both directions
    if (self->currentPhase >= 1.0)
    {
        self->currentPhase -= 1.0;
        self->wrapped = true; // Phase wrapped forward
    }
    else if (self->currentPhase < 0.0 && self->negativeWrappingEnabled)
    {
        self->currentPhase += 1.0;
        self->wrapped = true; // Phase wrapped backward
    }

    // Generate square wave: output +1.0 if phase is less than dutyCycle, else -1.0
    left = right = (self->currentPhase < self->dutyCycle) ? 1.0 : -1.0;
}

// Sets the duty cycle for PWM
void SquareOscillator::setDutyCycle(double value)
{
    dutyCycle = clamp(value, 0.1, 0.9);
}