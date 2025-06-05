#include "TriangleOscillator.h"
#include <vector>
#include <cmath>

TriangleOscillator::TriangleOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&TriangleOscillator::generateSample);
}

void TriangleOscillator::generateSample(Oscillator * /*osc*/, const double &phase, double &left, double &right)
{
    left = right = 4.0 * std::abs(phase - 0.5) - 1.0;
}