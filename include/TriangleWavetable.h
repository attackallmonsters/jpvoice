#pragma once

#include "WavetableOscillator.h"
#include "WaveformGenerator.h"

// Sine oscillator using wavetable lookup
class TriangleWavetable : public WavetableOscillator
{
public:
    TriangleWavetable() : WavetableOscillator("triangle") {};

protected:
    // Fills the given buffer with one cycle of a sine waveform
    void createWavetable(DSPBuffer &buffer, dsp_float frequency) override;
};
