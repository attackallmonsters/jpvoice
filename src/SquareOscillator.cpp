#include "SquareOscillator.h"
#include "clamp.h"

SquareOscillator::SquareOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SquareOscillator::getSampleIntern;
}

void SquareOscillator::getSampleIntern(DSPBase *dsp, double &left, double &right)
{
    SquareOscillator *osc = static_cast<SquareOscillator *>(dsp);

    // Compute phase increment and update phase
    osc->currentPhase += osc->phaseIncrement;
    osc->wrapped = false;

    // Wrap phase to stay within [0.0, 1.0) – works in both directions
    if (osc->currentPhase >= 1.0)
    {
        osc->currentPhase -= 1.0;
        osc->wrapped = true; // Phase wrapped forward
    }
    else if (osc->currentPhase < 0.0 && osc->negativeWrappingEnabled)
    {
        osc->currentPhase += 1.0;
        osc->wrapped = true; // Phase wrapped backward
    }

    // Generate square wave: output +1.0 if phase is less than dutyCycle, else -1.0
    left = right = (osc->currentPhase < osc->dutyCycle) ? 1.0 : -1.0;
}

// Sets the duty cycle for PWM
void SquareOscillator::setDutyCycle(double value)
{
    dutyCycle = clamp(value, 0.1, 0.9);
}