#include "TriangleOscillator.h"

TriangleOscillator::TriangleOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &TriangleOscillator::setSamplesIntern;
}

// Next sample block generation
void TriangleOscillator::setSamplesIntern(DSPObject *dsp)
{
    TriangleOscillator *osc = static_cast<TriangleOscillator *>(dsp);

    double phase = osc->currentPhase;
    bool wrapped = false;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Compute the phase increment based on the current frequency and sample rate
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

        // Triangle waveform: linear ramp up and down from -1 to +1
        // Formula: 4 * |x - 0.5| - 1 for x in [0, 1)
        osc->outBufferL[i] = osc->outBufferR[i] = 4.0 * std::abs(phase - 0.5) - 1.0;
    }

    osc->currentPhase = phase;
    osc->wrapped = wrapped;
}