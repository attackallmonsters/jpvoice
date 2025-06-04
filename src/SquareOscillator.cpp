#include "SquareOscillator.h"
#include "clamp.h"

SquareOscillator::SquareOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SquareOscillator::setSamplesIntern;
}

void SquareOscillator::setSamplesIntern(DSPObject *dsp)
{
    SquareOscillator *osc = static_cast<SquareOscillator *>(dsp);

    double phase = osc->currentPhase;
    bool wrapped = false;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        phase += osc->phaseIncrement;
        
        // Wrap phase to stay within [0.0, 1.0) – works in both directions
        if (phase >= 1.0)
        {
            phase -= 1.0;
            wrapped = true; // Phase wrapped forward
        }
        else if (phase < 0.0 && osc->negativeWrappingEnabled)
        {
            phase += 1.0;
            wrapped = true; // Phase wrapped backward
        }

        // Generate square wave: output +1.0 if phase is less than dutyCycle, else -1.0
        osc->outBufferL[i] = osc->outBufferR[i] = (phase < osc->dutyCycle) ? 1.0 : -1.0;
    }

    osc->currentPhase = phase;
    osc->wrapped = wrapped;
}

// Sets the duty cycle for PWM
void SquareOscillator::setDutyCycle(double value)
{
    dutyCycle = clamp(value, 0.1, 0.9);
}