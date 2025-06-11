#pragma once

#include "Oscillator.h"
#include "DSPBuffer.h"
#include <vector>
#include <cmath>
#include <memory>

// Abstract base class for all wavetable-based oscillators
class WavetableOscillator : public Oscillator
{
public:
    WavetableOscillator();

    // Initializes all wavetable buffers for multiple frequency ranges
    void Initialize() override;

protected:
    // Each subclass must define how to fill a table
    virtual void createWavetable(DSPBuffer &buffer, dsp_float frequency) = 0;

    // Frequency boundaries per LUT (e.g. per octave)
    std::vector<double> baseFrequencies;

    // Define the corresponding table size for each frequency range
    std::vector<size_t> tableSizes;

    // One wavetable buffer per frequency band
    std::vector<std::unique_ptr<DSPBuffer>> wavetableBuffers;

private:
    // Sample calculation without looking up vtable
    static void generateSample(Oscillator *, const dsp_float &, const dsp_float &, dsp_float &, dsp_float &);

    // Select appropriate wavetable for the given frequency
    DSPBuffer* selectTable(double frequency);
};
