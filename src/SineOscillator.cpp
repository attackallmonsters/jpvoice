#include "SineOscillator.h"

SineOscillator::SineOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SineOscillator::setSamplesIntern;
}

void SineOscillator::setSamplesIntern(DSPObject *dsp)
{
    SineOscillator *osc = static_cast<SineOscillator *>(dsp);

    double phase = osc->currentPhase;
    bool wrapped = false;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        // Update phase – phaseIncrement may be positive (normal FM) or negative (through-zero FM)
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

        // Generate sine wave: use 2π * phase
        // Direction of phase naturally controls direction of waveform
        osc->outBufferL[i] = osc->outBufferR[i] = std::sin(phase * 2.0 * M_PI);
    }

    osc->currentPhase = phase;
    osc->wrapped = wrapped;
}