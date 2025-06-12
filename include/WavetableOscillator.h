#pragma once

#include "Oscillator.h"
#include "DSPBuffer.h"
#include <vector>
#include <cmath>
#include <memory>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem> // C++17

// Internal voice struct used for detuned oscillators
struct WavetableVoice
{
    dsp_float phase;
    dsp_float detune_ratio;
    dsp_float amp_ratio;
    dsp_float gainL;
    dsp_float gainR;
};

// Abstract base class for all wavetable-based oscillators
class WavetableOscillator : public Oscillator
{
public:
    // Initializes all wavetable buffers for multiple frequency ranges
    void Initialize() override;

    // Sets the number of voices
    void setNumVoices(int count) override;

    // Sets the detune factor for the voices
    void setDetune(dsp_float value) override;

protected:
    // Ctor: expects an unique name for the waveform
    WavetableOscillator(const std::string formName);

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

    // Loads a wavetable
    bool load();

    // Saves a wavetable
    void save() const;

    // Select appropriate wavetable for the given frequency
    DSPBuffer *selectTable(double frequency);

    // The waveform name
    std::string waveformName;

    // stores the last wavetable to prevent lookup when frequency did not change
    const DSPBuffer *cachedTable = nullptr;
    size_t cachedTableSize = 0;
    dsp_float lastFrequency = -1.0;

    // The number of voices
    int numVoices = 1;
    std::vector<WavetableVoice> voices;

    // Voices detune
    dsp_float detune = 0.03;
};
