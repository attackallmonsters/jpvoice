#include <cstdlib>
#include "SupersawOscillator.h"

SupersawOscillator::SupersawOscillator()
{
    for (int i = 0; i < NUM_VOICES; ++i)
    {
        voices[i].phase = static_cast<double>(rand()) / RAND_MAX;
        voices[i].detune_ratio = DETUNE_RATIOS[i];
        voices[i].amp_ratio = AMP_RATIOS[i];
    }
    wrapped = false;
}

// Sets the detune factor
void SupersawOscillator::setDetune(double value)
{
    detune = clamp(value, 0.0, 1.0) * 0.25;
}

// Generates the next audio sample based on the current frequency and sample rate
void SupersawOscillator::getSample(double &left, double &right)
{
    left = right = 0.0;
    wrapped = false;
    snycDone = false;   // Avoids sync reset on every saw oscillator

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        SupersawVoice &v = voices[i];

        // Calculate per-voice frequency with detune
        double detune_factor = v.detune_ratio * detune;
        double voice_freq = calculatedFrequency * (1.0 + detune_factor);

        // Convert to phase increment for this voice
        double phaseInc = voice_freq / sampleRate;

        // Sawtooth signal in range [-1.0, +1.0]
        double val = 2.0 * v.phase - 1.0;

        // Update phase
        v.phase += phaseInc;
        if (v.phase >= 1.0 && !snycDone)
        {
            v.phase -= 1.0;
            wrapped = true;
            snycDone = true;
        }
        else if (v.phase < 0.0 && negativeWrappingEnabled)
        {
            v.phase += 1.0;
            wrapped = true; // Phase wrapped backward
            snycDone = true;
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
    left *= norm;
    right *= norm;
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