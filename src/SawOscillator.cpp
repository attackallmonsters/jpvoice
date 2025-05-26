#include "SawOscillator.h"

void SawOscillator::getSample(double &left, double &right)
{
    // Compute phase increment and update phase
    currentPhase += phaseIncrement;
    wrapped = false;

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
    
    // Raw sawtooth signal from -1.0 to +1.0
    left = right = 2.0 * currentPhase - 1.0;
}
