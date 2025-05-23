#pragma once

#include "Oscillator.h"
#include <random>

// Defines the available types of noise
enum class NoiseType
{
    White, // Uncorrelated white noise
    Pink   // Filtered pink noise (equal energy per octave)
};

// A subclass of Oscillator that generates noise instead of periodic waveforms.
// Supports white and pink noise generation.
class NoiseGenerator : public Oscillator
{
public:
    // Constructor: always initializes with white noise and distribution [-1, 1]
    NoiseGenerator();

    // Returns a single noise sample; frequency and sampleRate are unused but kept for compatibility
    double getSample() override;

    // Noise has no concept of phase, so this is a no-op
    void resetPhase() override;

    // Changes the current noise type (white or pink)
    void setType(NoiseType type);

private:
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
