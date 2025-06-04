#include "SineOscillator.h"

SineOscillator::SineOscillator()
{
    // to avoid vtable lookup
    computeSampleFunc = &SineOscillator::computeSampleFuncIntern;
}

void SineOscillator::computeSampleFuncIntern(Oscillator * /*osc*/, const double &phase, double &left, double &right)
{
    left = right = std::sin(phase * 2.0 * M_PI);
}