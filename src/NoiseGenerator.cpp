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
void NoiseGenerator::getSampleIntern(DSPBase *dsp, double &left, double &right)
{
    NoiseGenerator *osc = static_cast<NoiseGenerator *>(dsp);

    // Generate white noise sample in range [-1.0, 1.0]
    double white = osc->dist(osc->rng);

    // If white noise is selected, return it directly
    if (osc->noiseType == NoiseType::White)
    {
        left = right = white;
        return;
    }

    // --- Paul Kellet's Pink Noise Filter ---
    // Each term simulates a low-pass filter with different time constants
    // The coefficients are empirically derived for pink noise behavior
    osc->b0 = 0.99886 * osc->b0 + white * 0.0555179;
    osc->b1 = 0.99332 * osc->b1 + white * 0.0750759;
    osc->b2 = 0.96900 * osc->b2 + white * 0.1538520;
    osc->b3 = 0.86650 * osc->b3 + white * 0.3104856;
    osc->b4 = 0.55000 * osc->b4 + white * 0.5329522;
    osc->b5 = -0.7616 * osc->b5 - white * 0.0168980;
    double pink = osc->b0 + osc->b1 + osc->b2 + osc->b3 + osc->b4 + osc->b5 + osc->b6 + white * 0.5362;
    osc->b6 = white * 0.115926;

    // Scale output to normalize level (empirical factor)
    left = right = pink * 0.11;
}