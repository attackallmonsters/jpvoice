#include "SineOscillator.h"

void SineOscillator::getSample(double &left, double &right)
{
    // Reset wrap flag at start of each sample
    wrapped = false;

    // Update phase – phaseIncrement may be positive (normal FM) or negative (through-zero FM)
    currentPhase += phaseIncrement;

    // Wrap phase to stay within [0.0, 1.0) – works in both directions
    if (currentPhase >= 1.0)
    {
        currentPhase -= 1.0;
        wrapped = true; // Phase wrapped forward
    }
    else if (currentPhase < 0.0 && negativeWrappingEnabled)
    {
        currentPhase += 1.0;
        wrapped = true; // Phase wrapped backward
    }

    // Generate sine wave: use 2π * phase
    // Direction of phase naturally controls direction of waveform
    left = right = std::sin(currentPhase * 2.0 * M_PI);
}