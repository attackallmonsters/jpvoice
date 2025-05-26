#include "SineOscillator.h"

void SineOscillator::getSample(double &left, double &right)
{
    currentPhase += phaseIncrement;
    wrapped = false;

    if (currentPhase >= 1.0)
    {
        currentPhase -= 1.0;
        wrapped = true;
    }

    left = right = std::sin(currentPhase * 2.0 * M_PI);
}