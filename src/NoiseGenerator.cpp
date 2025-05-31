#include "NoiseGenerator.h"
#include <chrono> // Used to seed the RNG with high-resolution time

// Constructor: seeds RNG and sets noise type
NoiseGenerator::NoiseGenerator()
{
    // to avoid vtable lookup
    sampleFunc = &NoiseGenerator::getSampleIntern;

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
void NoiseGenerator::getSampleIntern(Oscillator *osc, double &left, double &right)
{
    NoiseGenerator *self = static_cast<NoiseGenerator *>(osc);

    // Generate white noise sample in range [-1.0, 1.0]
    double white = self->dist(self->rng);

    // If white noise is selected, return it directly
    if (self->noiseType == NoiseType::White)
    {
        left = right = white;
        return;
    }

    // --- Paul Kellet's Pink Noise Filter ---
    // Each term simulates a low-pass filter with different time constants
    // The coefficients are empirically derived for pink noise behavior
    self->b0 = 0.99886 * self->b0 + white * 0.0555179;
    self->b1 = 0.99332 * self->b1 + white * 0.0750759;
    self->b2 = 0.96900 * self->b2 + white * 0.1538520;
    self->b3 = 0.86650 * self->b3 + white * 0.3104856;
    self->b4 = 0.55000 * self->b4 + white * 0.5329522;
    self->b5 = -0.7616 * self->b5 - white * 0.0168980;
    double pink = self->b0 + self->b1 + self->b2 + self->b3 + self->b4 + self->b5 + self->b6 + white * 0.5362;
    self->b6 = white * 0.115926;

    // Scale output to normalize level (empirical factor)
    left = right = pink * 0.11;
}