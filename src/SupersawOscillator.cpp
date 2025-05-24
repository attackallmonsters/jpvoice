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
    detune = clamp(value, 0.0, 1.0);
}

// Generates the next audio sample based on the current frequency and sample rate
double SupersawOscillator::getSample()
{
    frequency = std::fmax(1.0, frequency);
    double sum = 0.0;
    wrapped = false;

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        SupersawVoice &v = voices[i];
        double detune_factor = v.detune_ratio * detune;
        double voice_freq = frequency * (1.0 + detune_factor);
        double phase_inc = voice_freq / sampleRate;
        double val = 2.0 * currentPhase - 1.0;

        currentPhase += phase_inc;
        if (currentPhase >= 1.0)
        {
            currentPhase -= 1.0;
            wrapped = true;
        }

        sum += val * v.amp_ratio;
    }

    return sum / static_cast<double>(NUM_VOICES);
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