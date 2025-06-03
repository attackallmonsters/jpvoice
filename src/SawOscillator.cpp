#include "SawOscillator.h"

SawOscillator::SawOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SawOscillator::setSamplesIntern;
}

// Next sample block generation
void SawOscillator::setSamplesIntern(DSP *dsp)
{
    SawOscillator *osc = static_cast<SawOscillator *>(dsp);

    double phase = osc->currentPhase;
    bool wrapped = false;

    for (int i = 0; i < DSP::blockSize; ++i)
    {
        // Compute phase increment and update phase
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

        // Raw sawtooth signal from -1.0 to +1.0
        osc->BufferLeft[i] = osc->BufferRight[i] = 2.0 * phase - 1.0;
    }

    osc->currentPhase = phase;
    osc->wrapped = wrapped;
}
