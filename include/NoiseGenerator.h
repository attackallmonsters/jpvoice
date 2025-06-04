#pragma once

#include "Oscillator.h"
#include "VoiceOptions.h"
#include "DSP.h"
#include "DSPObject.h"
#include <random>

// A subclass of Oscillator that generates noise instead of periodic waveforms.
// Supports white and pink noise generation.
class NoiseGenerator : public Oscillator
{
public:
    // Constructor: always initializes with white noise and distribution [-1, 1]
    NoiseGenerator();

    // Changes the current noise type (white or pink)
    void setType(NoiseType type);

private:
    // Sample calculation without looking up vtable
    static void computeSampleFuncIntern(Oscillator *, const double &, double &, double &);

    // Random number generator for white noise source
    std::mt19937 rng;

    // Uniform distribution for generating white noise values in range [-1.0, 1.0]
    std::uniform_real_distribution<double> dist{-1.0, 1.0};

    // Current type of noise being generated
    NoiseType noiseType;

    // State variables for pink noise filter (Paul Kellet method)
    // These variables store previous filter states to create 1/f pink noise characteristics
    double b0 = 0.0, b1 = 0.0, b2 = 0.0;
    double b3 = 0.0, b4 = 0.0, b5 = 0.0, b6 = 0.0;
};
