#pragma once

#include "WavetableOscillator.h"

// Sine oscillator using wavetable lookup
class SineWavetable : public WavetableOscillator
{
public:
    SineWavetable() = default;

protected:
    // Fills the given buffer with one cycle of a sine waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
