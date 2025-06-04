#include <cstdlib>
#include "SupersawOscillator.h"

SupersawOscillator::SupersawOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SupersawOscillator::setSamplesIntern;

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        voices[i].phase = static_cast<double>(rand()) / RAND_MAX;
        voices[i].detune_ratio = DETUNE_RATIOS[i];
        voices[i].amp_ratio = AMP_RATIOS[i];
    }
}

// Sets the detune factor
void SupersawOscillator::setDetune(double value)
{
    detune = clamp(value, 0.0, 1.0) * 0.25;
}

// Next sample block generation
void SupersawOscillator::setSamplesIntern(DSPObject *dsp)
{
    SupersawOscillator *osc = static_cast<SupersawOscillator *>(dsp); 

    bool wrapped = false;
    double left, right;

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        left = right = 0.0;

        for (int vIdx = 0; vIdx < NUM_VOICES; ++vIdx)
        {
            SupersawVoice &v = osc->voices[vIdx];

            // Calculate per-voice frequency with detune
            double detune_factor = v.detune_ratio * osc->detune;
            double voice_freq = osc->calculatedFrequency * (1.0 + detune_factor);

            // Convert to phase increment for this voice
            double phaseInc = voice_freq / DSP::sampleRate;

            // Sawtooth signal in range [-1.0, +1.0]
            double val = 2.0 * v.phase - 1.0;

            // Update phase
            v.phase += phaseInc;
            if (v.phase >= 1.0)
            {
                v.phase -= 1.0;
                wrapped = true;
            }
            else if (v.phase < 0.0 && osc->negativeWrappingEnabled)
            {
                v.phase += 1.0;
                wrapped = true;
            }

            // Panning based on voice index (-1.0 to +1.0)
            double pan = static_cast<double>(vIdx) / (NUM_VOICES - 1) * 2.0 - 1.0;
            double gainL = std::sqrt(0.5 * (1.0 - pan));
            double gainR = std::sqrt(0.5 * (1.0 + pan));

            // Accumulate stereo output
            left += val * v.amp_ratio * gainL;
            right += val * v.amp_ratio * gainR;
        }

        // Normalize output
        osc->outBufferL[i] = left * osc->norm;
        osc->outBufferR[i] = right * osc->norm;
    }

    osc->wrapped = wrapped;
}

// Resets the internal phase of all voices
void SupersawOscillator::resetPhase()
{
    Oscillator::resetPhase();

    for (auto &v : voices)
    {
        v.phase = 0.0;
    }
}