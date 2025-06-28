#include "WaveformGenerator.h"
#include "DSP.h"
#include "dsp_types.h"
#include "clamp.h"
#include <cmath>

void WaveformGenerator::generateWavetable(DSPBuffer &buffer,
                                          dsp_float baseFrequency,
                                          AmplitudeFunction amplitudeFunc,
                                          dsp_float harmonicBoost)
{
    size_t size = buffer.size();

    // Check for invalid input (no size, zero freq/sampleRate)
    if (size == 0 || baseFrequency <= 0.0)
    {
        DSP::log("WaveformGenerator::generateWavetable failed: invalid buffer size or invalid frequency");
        return;
    }

    // Nyquist frequency: we only include harmonics below this threshold
    const double nyquist = 0.5 * DSP::sampleRate;

    // Maximum number of harmonics allowed without aliasing
    int harmonics = static_cast<int>(nyquist / baseFrequency * (1 + clamp(harmonicBoost, 0, 1) * 9));

    // Constant for sine wave computation
    const double twoPi = 2.0 * M_PI;

    // Loop over each sample index in the buffer
    for (size_t i = 0; i < size; ++i)
    {
        // Convert index to phase in range [0.0, 1.0)
        double phase = static_cast<double>(i) / static_cast<double>(size - 1);

        double sample = 0.0;

        // Sum up all harmonics (from 1st to harmonics)
        for (int n = 1; n <= harmonics; ++n)
        {
            // Get amplitude for harmonic n
            double amp = amplitudeFunc(n);

            // Add sine component for harmonic n at current phase
            sample += amp * std::sin(twoPi * n * phase);
        }

        // Store computed sample in buffer (converted to DSP format)
        buffer[i] = static_cast<float>(DSP::zeroSubnormals(sample));
    }

    // Nomralisation
    // Find peak
    double peak = 0.0;
    for (size_t i = 0; i < size; ++i)
        peak = std::max(static_cast<float>(peak), std::abs(buffer[i]));

    // Normalize to ±1.0
    if (peak > 0.0)
    {
        for (size_t i = 0; i < size; ++i)
            buffer[i] /= peak;
    }
}
