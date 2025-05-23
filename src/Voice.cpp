#include <cmath>
#include "Voice.h"

// Constructor: initializes the voice with two oscillator instances.
// These oscillators are externally allocated and represent the carrier (osc1) and modulator (osc2).
Voice::Voice()
{
}

// Destructor: cleans up oscillator instances.
// It is assumed that osc1 and osc2 were allocated using 'new' and are not shared elsewhere.
Voice::~Voice()
{
    delete osc1;
    delete osc2;
}

// Osc1<>Osc2 mix: 0: Osc1 only; 1: Osc2 only
void Voice::setMix(double mix)
{
    if (mix < 0.0)
        mix = 0.0;

    if (mix > 1.0)
        mix = 1.0;

    oscmix = mix;
}

// Enables or disables frequency modulation (FM).
// When enabled, the output frequency of osc1 is modulated by the output of osc2.
void Voice::setFMEnabled(bool enabled)
{
    fmEnabled = enabled;
    paramChanged = true;
}

// Enables or disables oscillator synchronization.
// When enabled and osc1 wraps its phase, osc2 will be reset to phase 0.
void Voice::setSyncEnabled(bool enabled)
{
    syncEnabled = enabled;
    paramChanged = true;
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void Voice::setModIndex(double index)
{
    if (index < 0.0)
        index = 0.0;

    modulationIndex = index;
    paramChanged = true;
}

// Sets teh sample rate for signal calculation
void Voice::setSampleRate(double sampleRate)
{
    osc1->setSampleRate(sampleRate);
    osc2->setSampleRate(sampleRate);
}

// Sets the frequency of oscillator 1/carrier and oscillator 2/modulator
void Voice::setFrequencies(double frequencyOsc1, double frequencyOsc2)
{
    osc1->setFrequency(frequencyOsc1);
    osc2->setFrequency(frequencyOsc2);
}

// Applies changed paramters on low amplitude
void Voice::applyChangedParameters()
{
}

// Computes and returns a single audio sample from the voice.
// Takes two input frequencies (for osc1 and osc2) and the current sample rate.
// Depending on the modulation settings, the sample result is computed differently.
double Voice::getSample()
{
    // Step 1: Compute the output signal of the modulator oscillator (osc2).
    // This signal is used for FM or mixing, depending on the mode.
    double modSignal = osc2->getSample();

    // Step 2: Apply frequency modulation if enabled.
    // The carrier frequency (osc1) is modulated by the modulator signal,
    // scaled by the modulation index and by freq1 to keep it frequency-relative.
    double frequencyOsc1 = osc1->getFrequency();
    ;
    double effectiveFreq1 = frequencyOsc1;
    if (fmEnabled)
    {
        effectiveFreq1 += modSignal * modulationIndex * frequencyOsc1;
    }

    // Step 3: Generate the carrier signal sample (osc1), possibly frequency-modulated.
    double carrier = osc1->getSample();

    // Step 4: If oscillator sync is enabled and osc1 wrapped its phase this sample,
    // reset the phase of osc2 to re-align it.
    if (syncEnabled && osc1->hasWrapped())
    {
        osc2->resetPhase();
    }

    // Step 5: Determine final output depending on FM mode.
    double sample;
    if (fmEnabled)
    {
        // In FM mode, return only the modulated carrier signal.
        // This matches traditional FM synthesis behavior.
        sample = carrier;
    }
    else
    {
        // In normal mode, return a mixed signal from both oscillators.
        // The result is an equal-weight average of carrier and modulator signals.

        // Oscillator mix
        double amp_carrier = std::cos(oscmix * 0.5 * M_PI);
        double amp_modSignal = std::sin(oscmix * 0.5 * M_PI);

        sample = amp_carrier * carrier + amp_modSignal * modSignal;
    }

    // Quick fade out/fade in for param change
    if (paramChanged)
    {
        fadeCounter++;

        if (fadeCounter <= fadeLength)
        {
            // Fade out phase
            fadeValue = 1.0 - (fadeCounter / double(fadeLength));
        }
        else if (fadeCounter == fadeLength + 1)
        {
            applyChangedParameters();
        }
        else if (fadeCounter <= fadeLength * 2)
        {
            // Fade in phase
            fadeValue = (fadeCounter - fadeLength) / double(fadeLength);
        }
        else
        {
            // Finished
            paramChanged = false;
            fadeValue = 1.0;
            fadeCounter = 0;
        }

        sample *= fadeValue;
    }

    return sample;
}
