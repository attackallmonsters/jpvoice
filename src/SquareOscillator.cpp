#include "SquareOscillator.h"
#include "clamp.h"

SquareOscillator::SquareOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&SquareOscillator::generateSample);
}

void SquareOscillator::generateSample(Oscillator *osc, const double &phase, double &left, double &right)
{
    SquareOscillator *square = static_cast<SquareOscillator *>(osc);

    // Generate square wave: output +1.0 if phase is less than dutyCycle, else -1.0
    left = right = (phase < square->dutyCycle) ? 1.0 : -1.0;
}

// Sets the duty cycle for PWM
void SquareOscillator::setDutyCycle(double value)
{
    dutyCycle = clamp(value, 0.05, 0.95);
}