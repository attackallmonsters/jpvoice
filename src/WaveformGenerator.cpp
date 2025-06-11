#include "WaveformGenerator.h"
#include "DSP.h"
#include "dsp_types.h"
#include <cmath>

void WaveformGenerator::generateWavetable(DSPBuffer &buffer,
                                          dsp_float baseFrequency,
                                          AmplitudeFunction amplitudeFunc,
                                          int maxHarmonics)
{
    size_t size = buffer.size();

    // Check for invalid input (no size, zero freq/sampleRate)
    if (size == 0 || DSP::sampleRate <= 0.0 || baseFrequency <= 0.0)
        return;

    // Ensure buffer has correct size (resize to itself to be safe)
    buffer.resize(size);

    // Nyquist frequency: we only include harmonics below this threshold
    const dsp_float nyquist = 0.5 * DSP::sampleRate;

    // Maximum number of harmonics allowed without aliasing
    int harmonics = static_cast<int>(nyquist / baseFrequency);

    if (maxHarmonics > 0 && maxHarmonics < harmonics)
        harmonics = maxHarmonics;

    // Constant for sine wave computation
    const dsp_float twoPi = 2.0 * M_PI;

    // Loop over each sample index in the buffer
    for (size_t i = 0; i < size; ++i)
    {
        // Convert index to phase in range [0.0, 1.0)
        dsp_float phase = static_cast<dsp_float>(i) / static_cast<dsp_float>(size);

        dsp_float sample = 0.0;

        // Sum up all harmonics (from 1st to harmonics)
        for (int n = 1; n <= harmonics; ++n)
        {
            // Get amplitude for harmonic n
            dsp_float amp = amplitudeFunc(n, harmonics);

            // Add sine component for harmonic n at current phase
            sample += amp * std::sin(twoPi * n * phase);
        }

        // Store computed sample in buffer (converted to DSP format)
        buffer[i] = static_cast<dsp_float>(sample);
    }
}
