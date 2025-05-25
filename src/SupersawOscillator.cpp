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
void SupersawOscillator::getSample(double &left, double &right)
{
    wrapped = false;

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        SupersawVoice &v = voices[i];
        double detune_factor = v.detune_ratio * detune;
        double voice_freq = calculatedFrequency * (1.0 + detune_factor);
        double phase_inc = voice_freq / sampleRate;

        double val = 2.0 * v.phase - 1.0;

        v.phase += phase_inc;
        if (v.phase >= 1.0)
        {
            v.phase -= 1.0;
            wrapped = true;
        }

        double pan = static_cast<double>(i) / (NUM_VOICES - 1) * 2.0 - 1.0;
        double gainL = std::sqrt(0.5 * (1.0 - pan));
        double gainR = std::sqrt(0.5 * (1.0 + pan));

        left  += val * v.amp_ratio * gainL;
        right += val * v.amp_ratio * gainR;
    }
    
    left = left * norm;
    right = right * norm;
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