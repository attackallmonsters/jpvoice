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

// Generates the next audio sample based on the current frequency and sample rate
double SupersawOscillator::getSample()
{
    if (sampleRate <= 0.0)
        sampleRate = 44100.0;

    frequency = std::fmax(1.0, frequency);
    double sum = 0.0;
    wrapped = false;

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        Voice &v = voices[i];
        double detune_factor = v.detune_ratio * detune;
        double voice_freq = frequency * (1.0 + detune_factor);
        double phase_inc = voice_freq / sampleRate;
        double t = v.phase;
        double val = 2.0 * t - 1.0;

        v.phase += phase_inc;
        if (v.phase >= 1.0)
        {
            v.phase -= 1.0;
            wrapped = true;
        }

        sum += val * v.amp_ratio;
    }

    return sum / static_cast<double>(NUM_VOICES);
}

// Resets the internal phase of all voices
void SupersawOscillator::resetPhase()
{
    for (auto &v : voices)
    {
        v.phase = 0.0;
    }
    wrapped = false;
}

// Returns true if any voice's phase wrapped in the last call
bool SupersawOscillator::hasWrapped() const
{
    return wrapped;
}
