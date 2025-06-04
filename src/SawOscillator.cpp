#include "SawOscillator.h"

SawOscillator::SawOscillator()
{
    // to avoid vtable lookup
    computeSampleFunc = &SawOscillator::computeSampleFuncIntern;
}

void SawOscillator::computeSampleFuncIntern(Oscillator * /*osc*/, const double &phase, double &left, double &right)
{
    // Raw sawtooth signal from -1.0 to +1.0
    left = right = 2.0 * phase - 1.0;
}
