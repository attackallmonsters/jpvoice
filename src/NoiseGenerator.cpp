#include "NoiseGenerator.h"
#include <chrono> // Used to seed the RNG with high-resolution time

// Constructor: seeds RNG and sets noise type
NoiseGenerator::NoiseGenerator()
{
    // WHite noise is default
    setType(NoiseType::White);

    // Seed RNG using the system clock to ensure randomness across runs
    rng.seed(static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));
}

// Allows dynamic switching between white and pink noise
void NoiseGenerator::setType(NoiseType type)
{
    noiseType = type;
}

// Returns one noise sample, depending on the selected type (white or pink)
double NoiseGenerator::getSample()
{
    // Generate white noise sample in range [-1.0, 1.0]
    double white = dist(rng);

    // If white noise is selected, return it directly
    if (noiseType == NoiseType::White)
    {
        return white;
    }

    // --- Paul Kellet's Pink Noise Filter ---
    // Each term simulates a low-pass filter with different time constants
    // The coefficients are empirically derived for pink noise behavior
    b0 = 0.99886 * b0 + white * 0.0555179;
    b1 = 0.99332 * b1 + white * 0.0750759;
    b2 = 0.96900 * b2 + white * 0.1538520;
    b3 = 0.86650 * b3 + white * 0.3104856;
    b4 = 0.55000 * b4 + white * 0.5329522;
    b5 = -0.7616 * b5 - white * 0.0168980;
    double pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362;
    b6 = white * 0.115926;

    // Scale output to normalize level (empirical factor)
    return pink * 0.11;
}