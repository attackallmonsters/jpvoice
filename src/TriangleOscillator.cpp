#include "TriangleOscillator.h"

void TriangleOscillator::getSample(double &left, double &right)
{
    // Compute the phase increment based on the current frequency and sample rate
    currentPhase += phaseIncrement;
    wrapped = false;

    // Wrap phase to stay in [0.0, 1.0)
    if (currentPhase >= 1.0)
    {
        currentPhase -= 1.0;
        wrapped = true;
    }

    // Triangle waveform: linear ramp up and down from -1 to +1
    // Formula: 4 * |x - 0.5| - 1 for x in [0, 1)
    left = right = 4.0 * std::abs(currentPhase - 0.5) - 1.0;
}