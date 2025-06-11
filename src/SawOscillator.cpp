#include "SawOscillator.h"
#include "dsp_types.h"

SawOscillator::SawOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&SawOscillator::generateSample);
}

void SawOscillator::generateSample(Oscillator * /*osc*/, const dsp_float & /*frequency*/, const dsp_float &phase, dsp_float &left, dsp_float &right)
{
    // Raw sawtooth signal from -1.0 to +1.0
    left = right = 2.0 * phase - 1.0;
}
