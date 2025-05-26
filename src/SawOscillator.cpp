#include "SawOscillator.h"

void SawOscillator::getSample(double &left, double &right)
{
    // Compute phase increment and update phase
    currentPhase += phaseIncrement;
    wrapped = false;

    if (currentPhase >= 1.0)
    {
        currentPhase -= 1.0;
        wrapped = true;
    }
    
    // Raw sawtooth signal from -1.0 to +1.0
    left = right = 2.0 * currentPhase - 1.0;
}
