#include "SquareOscillator.h"
#include "clamp.h"

void SquareOscillator::getSample(double &left, double &right)
{
    // Compute phase increment and update phase
    currentPhase += phaseIncrement;
    wrapped = false;

    // Wrap phase to stay in [0.0, 1.0)
    if (currentPhase >= 1.0)
    {
        currentPhase -= 1.0;
        wrapped = true;
    }

    // Generate square wave: output +1.0 if phase is less than dutyCycle, else -1.0
    left = right = (currentPhase < dutyCycle) ? 1.0 : -1.0;
}

// Sets the duty cycle for PWM
void SquareOscillator::setDutyCycle(double value)
{
    dutyCycle = clamp(value, 0.1, 0.9);
}