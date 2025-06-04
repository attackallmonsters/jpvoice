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

        double pan = static_cast<double>(i) / (NUM_VOICES - 1) * 2.0 - 1.0;
        voices[i].gainL = std::sqrt(0.5 * (1.0 - pan));
        voices[i].gainR = std::sqrt(0.5 * (1.0 + pan));
    }
}

// Sets the detune factor
void SupersawOscillator::setDetune(double value)
{
    detune = clamp(value, 0.0, 1.0) * 0.25;
}

void SupersawOscillator::computeSampleFuncIntern(Oscillator *osc, const double & /*phase*/, double &left, double &right)
{
    SupersawOscillator *ssaw = static_cast<SupersawOscillator *>(osc);

    double sawleft = 0.0, sawright = 0.0;
    double sr = DSP::sampleRate;

    for (int vIdx = 0; vIdx < NUM_VOICES; ++vIdx)
    {
        SupersawVoice &v = ssaw->voices[vIdx];

        // Calculate per-voice frequency with detune
        double detune_factor = v.detune_ratio * ssaw->detune;
        double voice_freq = ssaw->calculatedFrequency * (1.0 + detune_factor);

        // Convert to phase increment for this voice
        double phaseInc = voice_freq / sr;

        // Sawtooth signal in range [-1.0, +1.0]
        double val = 2.0 * v.phase - 1.0;

        // Update phase
        v.phase += phaseInc;
        if (v.phase >= 1.0)
        {
            v.phase -= 1.0;
            ssaw->wrapped = true;
        }
        else if (v.phase < 0.0 && ssaw->negativeWrappingEnabled)
        {
            v.phase += 1.0;
            ssaw->wrapped = true;
        }

        // Accumulate stereo output
        sawleft += val * v.amp_ratio * v.gainL;
        sawright += val * v.amp_ratio * v.gainR;
    }

    // Normalize output
    left = sawleft * ssaw->norm;
    right = sawright * ssaw->norm;
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