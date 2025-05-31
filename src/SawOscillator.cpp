#include "SawOscillator.h"

SawOscillator::SawOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SawOscillator::getSampleIntern;
}

void SawOscillator::getSampleIntern(DSPBase* dsp, double &left, double &right)
{
    SawOscillator *osc = static_cast<SawOscillator *>(dsp);

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
    
    // Raw sawtooth signal from -1.0 to +1.0
    left = right = 2.0 * osc->currentPhase - 1.0;
}
