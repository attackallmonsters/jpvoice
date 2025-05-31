#include <cstdlib>
#include "SupersawOscillator.h"

SupersawOscillator::SupersawOscillator()
{
    // to avoid vtable lookup
    sampleFunc = &SupersawOscillator::getSampleIntern;

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

// Generates the next audio sample based on the current frequency and sample rate
void SupersawOscillator::getSampleIntern(DSPBase *dsp, double &left, double &right)
{
    SupersawOscillator *osc = static_cast<SupersawOscillator *>(dsp);

    left = right = 0.0;
    osc->wrapped = false;
    osc->snycDone = false;   // Avoids sync reset on every saw oscillator

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        SupersawVoice &v = osc->voices[i];

        // Calculate per-voice frequency with detune
        double detune_factor = v.detune_ratio * osc->detune;
        double voice_freq = osc->calculatedFrequency * (1.0 + detune_factor);

        // Convert to phase increment for this voice
        double phaseInc = voice_freq / osc->sampleRate;

        // Sawtooth signal in range [-1.0, +1.0]
        double val = 2.0 * v.phase - 1.0;

        // Update phase
        v.phase += phaseInc;
        if (v.phase >= 1.0 && !osc->snycDone)
        {
            v.phase -= 1.0;
            osc->wrapped = true;
            osc->snycDone = true;
        }
        else if (v.phase < 0.0 && osc->negativeWrappingEnabled)
        {
            v.phase += 1.0;
            osc->wrapped = true; // Phase wrapped backward
            osc->snycDone = true;
        }

        // Panning based on voice index (-1.0 to +1.0)
        double pan = static_cast<double>(i) / (NUM_VOICES - 1) * 2.0 - 1.0;
        double gainL = std::sqrt(0.5 * (1.0 - pan));
        double gainR = std::sqrt(0.5 * (1.0 + pan));

        // Accumulate stereo output
        left += val * v.amp_ratio * gainL;
        right += val * v.amp_ratio * gainR;
    }

    // Normalize output
    left *= osc->norm;
    right *= osc->norm;
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