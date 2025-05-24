#include <cmath>
#include "Voice.h"
#include "tool.h"

// Constructor: initializes the voice with two oscillator instances.
// These oscillators are externally allocated and represent the carrier (carrier) and modulator (modulator).
Voice::Voice()
{
    setCarrierOscillatorType(CarrierOscillatiorType::Supersaw);
    setModulatorOscillatorType(ModulatorOscillatorType::Sine);
}

// Destructor: cleans up oscillator instances.
// It is assumed that carrier and modulator were allocated using 'new' and are not shared elsewhere.
Voice::~Voice()
{
    delete carrier;
    delete modulator;
    delete noise;
}

// Enables or disables frequency modulation (FM).
// When enabled, the output frequency of carrier is modulated by the output of modulator.
void Voice::setFMEnabled(bool enabled)
{
    fmEnabled = enabled;
    paramChanged = true;
}

// Enables or disables oscillator synchronization.
// When enabled and carrier wraps its phase, modulator will be reset to phase 0.
void Voice::setSyncEnabled(bool enabled)
{
    syncEnabled = enabled;
    paramChanged = true;
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void Voice::setModIndex(double index)
{
    modulationIndex = clampmin(index, 0.0);
    paramChanged = true;
}

// Sets teh sample rate for signal calculation
void Voice::setSampleRate(double sampleRate)
{
    double rate = clamp(sampleRate, 41100.0, 96000.0);
    carrier->setSampleRate(rate);
    modulator->setSampleRate(rate);
}

// Sets the frequency of oscillator 1/carrier
void Voice::setFrequencyCarrier(double frequency)
{
    carrier->setFrequency(frequency);
    frequencyCarrier = frequency;
}

// Sets the frequency of oscillator 2/modulator
void Voice::setFrequencyModulator(double frequency)
{
    modulator->setFrequency(frequency);
    frequencyModulator = frequency;
}

// Sets the detune factor
void Voice::setDetune(float value)
{
    carrier->setDetune(value);
    modulator->setDetune(value);
}

// Sets the volume level of the oscillators
void Voice::setOscillatorMix(double mix)
{
    oscmix = clamp(mix, 0.0, 1.0);
}

// Sets the volume level of the noise generator
void Voice::setNoiseMix(double mix)
{
    noisemix = clamp(mix, 0.0, 1.0);
}

// Assigns the carrier oscillator
void Voice::setCarrierOscillatorType(CarrierOscillatiorType oscillatorType)
{
    switch (oscillatorType)
    {
    case CarrierOscillatiorType::Supersaw:
        carrier = supersawCarrier;
        break;

    default:
        break;
    }
}

// Assigns the modulation oscillator
void Voice::setModulatorOscillatorType(ModulatorOscillatorType oscillatorType)
{
    switch (oscillatorType)
    {
    case ModulatorOscillatorType::Sine:
        modulator = sineModulator;
        break;

    default:
        break;
    }
}

// Changes the current noise type (white or pink)
void Voice::setNoiseType(NoiseType type) {}

// Applies changed paramters on low amplitude
void Voice::applyChangedParameters()
{
}

// Computes and returns a single audio sample from the voice.
// Takes two input frequencies (for carrier and modulator) and the current sample rate.
// Depending on the modulation settings, the sample result is computed differently.
double Voice::getSample()
{
    // Step 1: Compute the output signal of the modulator oscillator (modulator).
    // This signal is used for FM or mixing, depending on the mode.
    double modulatorSample = modulator->getSample();

    // Step 2: Apply frequency modulation if enabled.
    // The carrier frequency (carrier) is modulated by the modulator signal,
    // scaled by the modulation index and by freq1 to keep it frequency-relative.
    double frequencyOsc1 = carrier->getFrequency();
    ;
    double effectiveFreq1 = frequencyOsc1;
    if (fmEnabled)
    {
        effectiveFreq1 += modulatorSample * modulationIndex * frequencyOsc1;
    }

    // Step 3: Generate the carrier signal sample (carrier), possibly frequency-modulated.
    double carrierSample = carrier->getSample();

    // Step 4: If oscillator sync is enabled and carrier wrapped its phase this sample,
    // reset the phase of modulator to re-align it.
    if (syncEnabled && carrier->hasWrapped())
    {
        modulator->resetPhase();
    }

    // Step 5: Determine final output depending on FM mode.
    double sample;
    if (fmEnabled)
    {
        // In FM mode, return only the modulated carrier signal.
        // This matches traditional FM synthesis behavior.
        sample = carrierSample;
    }
    else
    {
        // In normal mode, return a mixed signal from both oscillators.
        // The result is an equal-weight average of carrier and modulator signals.

        // Oscillator mix
        double amp_carrier = std::cos(oscmix * 0.5 * M_PI);
        double amp_modSignal = std::sin(oscmix * 0.5 * M_PI);

        sample = amp_carrier * carrierSample + amp_modSignal * modulatorSample;
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
