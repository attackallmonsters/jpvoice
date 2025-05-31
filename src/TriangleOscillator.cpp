#include "TriangleOscillator.h"

TriangleOscillator::TriangleOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &TriangleOscillator::getSampleIntern;
}

void TriangleOscillator::getSampleIntern(Oscillator *osc, double &left, double &right)
{
    // Compute the phase increment based on the current frequency and sample rate
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

    // Triangle waveform: linear ramp up and down from -1 to +1
    // Formula: 4 * |x - 0.5| - 1 for x in [0, 1)
    left = right = 4.0 * std::abs(osc->currentPhase - 0.5) - 1.0;
}