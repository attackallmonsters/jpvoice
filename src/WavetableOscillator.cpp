#include "WavetableOscillator.h"

WavetableOscillator::WavetableOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&WavetableOscillator::generateSample);

    // Define the base frequencies at which a new wavetable is used
    // These roughly correspond to octave steps starting from 20 Hz
    baseFrequencies = {20.0, 40.0, 160.0, 640.0, 2560.0};

    // Define the corresponding table size for each frequency range
    // Higher frequencies require higher resolution to avoid interpolation artifacts
    tableSizes = {1024, 2048, 4096, 8192, 16384};
}

void WavetableOscillator::Initialize()
{
    Oscillator::Initialize();
    
    // Clear any existing tables
    wavetableBuffers.clear();
    wavetableBuffers.reserve(tableSizes.size());

    for (size_t i = 0; i < tableSizes.size(); ++i)
    {
        size_t size = tableSizes[i];
        dsp_float freq = baseFrequencies[i];

        // Create a new DSPBuffer instance and resize it to the desired table size
        auto buffer = std::make_unique<DSPBuffer>();
        buffer->resize(size);

        // Let the subclass generate the actual waveform data
        createWavetable(*buffer, freq);

        // Store the buffer for later use (e.g., waveform lookup)
        wavetableBuffers.push_back(std::move(buffer));
    }
}

DSPBuffer *WavetableOscillator::selectTable(double frequency)
{
    for (size_t i = baseFrequencies.size(); i-- > 0;)
    {
        if (frequency >= baseFrequencies[i])
            return wavetableBuffers[i].get();
    }

    // Fallback
    return wavetableBuffers.front().get();
}

void WavetableOscillator::generateSample(Oscillator *osc, const dsp_float &frequency, const dsp_float &phase, dsp_float &left, dsp_float &right)
{
    WavetableOscillator *wto = static_cast<WavetableOscillator *>(osc);

    // Select appropriate wavetable for the given frequency
    const DSPBuffer &table = *(wto->selectTable(frequency));
    const size_t tableSize = table.size();

    // Calculate fractional index into the table
    double index = static_cast<double>(phase) * static_cast<double>(tableSize);
    size_t i0 = static_cast<size_t>(index) % tableSize; // current index
    size_t i1 = (i0 + 1) % tableSize;                   // next index
    double frac = index - static_cast<double>(i0);      // fractional part

    // Linear interpolation
    double sample =
        (1.0 - frac) * table[i0] +
        frac * table[i1];

    // Write to outputs
    left = right = static_cast<dsp_float>(sample);
}
