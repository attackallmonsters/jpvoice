#include "SineOscillator.h"

SineOscillator::SineOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SineOscillator::getSampleIntern;
}

void SineOscillator::getSampleIntern(Oscillator *osc, double &left, double &right)
{
    // Reset wrap flag at start of each sample
    osc->wrapped = false;

    // Update phase – phaseIncrement may be positive (normal FM) or negative (through-zero FM)
    osc->currentPhase += osc->phaseIncrement;

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

    // Generate sine wave: use 2π * phase
    // Direction of phase naturally controls direction of waveform
    left = right = std::sin(osc->currentPhase * 2.0 * M_PI);
}