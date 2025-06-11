#include <cstdlib>
#include "SupersawOscillator.h"
#include "dsp_types.h"

SupersawOscillator::SupersawOscillator()
{
    // to avoid vtable lookup
    registerSampleGenerator(&SupersawOscillator::generateSample);

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        voices[i].phase = static_cast<dsp_float>(rand()) / RAND_MAX;
        voices[i].detune_ratio = DETUNE_RATIOS[i];
        voices[i].amp_ratio = AMP_RATIOS[i];

        dsp_float pan = static_cast<dsp_float>(i) / (NUM_VOICES - 1) * 2.0 - 1.0;
        voices[i].gainL = std::sqrt(0.5 * (1.0 - pan));
        voices[i].gainR = std::sqrt(0.5 * (1.0 + pan));
    }
}

// Sets the detune factor
void SupersawOscillator::setDetune(dsp_float value)
{
    detune = clamp(value, 0.0, 1.0) * 0.25;
}

void SupersawOscillator::generateSample(Oscillator *osc, const dsp_float & /*frequency*/, const dsp_float & /*phase*/, dsp_float &left, dsp_float &right)
{
    SupersawOscillator *ssaw = static_cast<SupersawOscillator *>(osc);

    dsp_float sawleft = 0.0, sawright = 0.0;
    dsp_float sr = DSP::sampleRate;
    dsp_float detune = ssaw->detune;
    dsp_float freq = ssaw->calculatedFrequency;

    for (int vIdx = 0; vIdx < NUM_VOICES; ++vIdx)
    {
        SupersawVoice &v = ssaw->voices[vIdx];

        // Calculate per-voice frequency with detune
        dsp_float detune_factor = v.detune_ratio * detune;
        dsp_float voice_freq = freq * (1.0 + detune_factor);

        // Convert to phase increment for this voice
        dsp_float phaseInc = voice_freq / sr;

        // Sawtooth signal in range [-1.0, +1.0]
        dsp_float val = 2.0 * v.phase - 1.0;

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