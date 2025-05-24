#include "SineOscillator.h"

double SineOscillator::getSample()
{
   double phaseIncrement = frequency / sampleRate;
    phase += phaseIncrement;
    if (phase >= 1.0) phase -= 1.0;

    double x = std::sin(phase * 2.0 * M_PI);
    return x - 0.1 * std::pow(x, 3);
}