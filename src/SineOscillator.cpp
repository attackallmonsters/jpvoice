#include "SineOscillator.h"

void SineOscillator::getSample(double &left, double &right)
{
    double phaseIncrement = calculatedFrequency / sampleRate;
    currentPhase += phaseIncrement;
    if (currentPhase >= 1.0)
        currentPhase -= 1.0;

    double x = std::sin(currentPhase * 2.0 * M_PI);
    left = right = x - 0.1 * std::pow(x, 3);
}