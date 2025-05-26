#include <cmath>
#include "Voice.h"
#include "clamp.h"

// Constructor: initializes the voice with two oscillator instances.
// These oscillators are externally allocated and represent the carrier (carrier) and modulator (modulator).
Voice::Voice() : carrier(nullptr), modulator(nullptr)
{
    carrier = supersawCarrier;
    modulator = sineModulator;
}

// Destructor: cleans up oscillator instances.
// It is assumed that carrier and modulator were allocated using 'new' and are not shared elsewhere.
Voice::~Voice()
{
    delete noise;
    delete supersawCarrier;
    delete sineCarrier;
    delete sineModulator;
    delete sawCarrier;
    delete sawModulator;
    delete squareCarrier;
    delete squareModulator;
    delete trianlgeCarrier;
    delete triangleModulator;
}

// Enables or disables frequency modulation (FM).
// When enabled, the output frequency of carrier is modulated by the output of modulator.
void Voice::setFMEnabled(bool enabled)
{
    fmEnabled = enabled;

    if (!enabled)
    {
        carrier->setFrequency(frequency);
    }
}

// Enables or disables oscillator synchronization.
// When enabled and carrier wraps its phase, modulator will be reset to phase 0.
void Voice::setSyncEnabled(bool enabled)
{
    syncEnabled = enabled;
}

// Sets the duty cycle
void Voice::setDutyCycle(double cycle)
{
    double dutyCycle = clamp(cycle, 0.0, 1.0);
    squareModulator->setDutyCycle(dutyCycle);
    squareCarrier->setDutyCycle(dutyCycle);
}

// Sets the pitch offset for the modulator
void Voice::setPitchOffset(double offset)
{
    pitchOffset = clamp(offset, -24.0, 24.0);
    modulator->setPitchOffset(pitchOffset);
}

// Sets the fine tunig for the modulator
void Voice::setFineTune(double fine)
{
    fineTune = clamp(fine, -100.0, 100.0);
    modulator->setFineTune(fineTune);
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void Voice::setFMModIndex(double index)
{
    modulationIndex = clampmin(index, 0.0);
}

// Sets teh sample rate for signal calculation
void Voice::setSampleRate(double sampleRate)
{
    double rate = clamp(sampleRate, 41100.0, 96000.0);
    noise->setSampleRate(rate);
    supersawCarrier->setSampleRate(rate);
    sineCarrier->setSampleRate(rate);
    sineModulator->setSampleRate(rate);
    sawCarrier->setSampleRate(rate);
    sawModulator->setSampleRate(rate);
    squareCarrier->setSampleRate(rate);
    squareModulator->setSampleRate(rate);
    trianlgeCarrier->setSampleRate(rate);
    triangleModulator->setSampleRate(rate);
}

// Sets the current frequency
void Voice::setFrequency(double f)
{
    carrier->setFrequency(f);
    modulator->setFrequency(f);
    frequency = f;
}

// Sets the detune factorjpvoice_tilde_sync
void Voice::setDetune(double value)
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
    double f = (carrier) ? carrier->getFrequency() : 0.0;

    switch (oscillatorType)
    {
    case CarrierOscillatiorType::Supersaw:
        carrierTmp = supersawCarrier;
        break;
    case CarrierOscillatiorType::Saw:
        carrierTmp = sawCarrier;
        break;
    case CarrierOscillatiorType::Square:
        carrierTmp = squareCarrier;
        break;
    case CarrierOscillatiorType::Triangle:
        carrierTmp = trianlgeCarrier;
        break;
    case CarrierOscillatiorType::Sine:
        carrierTmp = sineCarrier;
        break;
    default:
        carrierTmp = supersawCarrier;
        break;
    }

    if (carrierTmp == carrier)
    {
        return;
    }

    carrierTmp->setFrequency(f);
    applyOscillators = true;
}

// Assigns the modulation oscillator
void Voice::setModulatorOscillatorType(ModulatorOscillatorType oscillatorType)
{
    double f = (modulator) ? modulator->getFrequency() : 0.0;

    switch (oscillatorType)
    {
    case ModulatorOscillatorType::Saw:
        modulatorTmp = sawModulator;
        break;
    case ModulatorOscillatorType::Square:
        modulatorTmp = squareModulator;
        break;
    case ModulatorOscillatorType::Triangle:
        modulatorTmp = triangleModulator;
        break;
    case ModulatorOscillatorType::Sine:
        modulatorTmp = sineModulator;
        break;
    default:
        modulatorTmp = sineModulator;
        break;
    }

    if (modulatorTmp == modulator)
    {
        return;
    }

    modulatorTmp->setFrequency(f);
    modulatorTmp->setPitchOffset(pitchOffset);
    modulatorTmp->setFineTune(fineTune);
    applyOscillators = true;
}

// Changes the current noise type (white or pink)
void Voice::setNoiseType(NoiseType type)
{
    noise->setType(type);
}

// Computes and returns a single audio sample from the voice.
// Takes two input frequencies (for carrier and modulator) and the current sample rate.
// Depending on the modulation settings, the sample result is computed differently.
void Voice::getSample(double &left, double &right)
{
    // --- Step 1: Get stereo signal from the modulator oscillator ---
    // This signal will be used either for frequency modulation or direct audio mixing.
    modulator->getSample(modulatorSampleLeft, modulatorSampleRight);

    // --- Step 2: Apply Frequency Modulation if enabled ---
    // Use the average (mono) value of the stereo modulator signal
    // to modulate the frequency of the carrier oscillator.
    if (fmEnabled)
    {
        double frequencyCarrier = frequency;                                         // Get current frequency
        double modulatorSample = 0.5 * (modulatorSampleLeft + modulatorSampleRight); // Convert to mono
        frequencyCarrier += modulatorSample * modulationIndex * frequency;           // Apply FM
        carrier->setCalculatedFrequency(frequencyCarrier);                           // Set the modulated frequency
    }

    // --- Step 3: Generate the stereo output from the carrier oscillator ---
    // This will either be used directly (in FM mode) or mixed with the modulator signal.
    carrier->getSample(carrierSampleLeft, carrierSampleRight);

    // --- Step 4: Sync handling ---
    // If sync is enabled and the carrier oscillator has wrapped (phase reset),
    // we reset the phase of the modulator oscillator to align it.
    if (syncEnabled && carrier->hasWrapped())
    {
        modulator->resetPhase();
    }

    // --- Step 5.1: Oscillator mixing or FM-only output ---
    if (fmEnabled)
    {
        // FM Mode: Only output the carrier oscillator's stereo signal.
        // The modulator is not heard directly (as in classic FM synths).
        mixSampleLeft = carrierSampleLeft;
        mixSampleRight = carrierSampleRight;
    }

    // --- Step 5.2: Mix the carrier and modulator signals using an equal-power crossfade.
    // The oscmix parameter defines the mix balance (0.0 = only carrier, 1.0 = only modulator).
    amp_carrier = std::cos(oscmix * 0.5 * M_PI);
    amp_modulator = std::sin(oscmix * 0.5 * M_PI);

    mixSampleLeft = amp_carrier * carrierSampleLeft + amp_modulator * modulatorSampleLeft;
    mixSampleRight = amp_carrier * carrierSampleRight + amp_modulator * modulatorSampleRight;

    // --- Step 6: Add noise to the signal, if enabled ---
    if (noisemix > 0)
    {
        // Get stereo signal from the noise generator.
        noise->getSample(noiseSampleLeft, noiseSampleRight);

        // Apply another equal-power crossfade between the oscillator signal and noise.
        // noisemix controls how much noise is mixed in (0.0 = no noise, 1.0 = only noise).
        amp_oscmix = std::cos(noisemix * 0.5 * M_PI);
        amp_noise = std::sin(noisemix * 0.5 * M_PI);

        mixSampleLeft = amp_oscmix * mixSampleLeft + amp_noise * noiseSampleLeft;
        mixSampleRight = amp_oscmix * mixSampleRight + amp_noise * noiseSampleRight;
    }

    // --- Step 7: Smooth fade-out/fade-in when parameters change ---
    if (applyOscillators)
    {
        fadeCounter++;

        if (fadeCounter <= fadeLength)
        {
            // Fade out: Linearly reduce output volume
            fadeValue = 1.0 - (double(fadeCounter) / fadeLength);
        }
        else if (fadeCounter == fadeLength + 1)
        {
            if (carrier != carrierTmp)
            {
                carrier = carrierTmp;
            }

            if (modulator != modulatorTmp)
            {
                modulator = modulatorTmp;
            }
        }
        else if (fadeCounter <= fadeLength * 2)
        {
            // Fade in: Linearly restore output volume
            fadeValue = (double(fadeCounter - fadeLength) / fadeLength);
        }
        else
        {
            // Fade process complete: reset state
            applyOscillators = false;
            fadeValue = 1.0;
            fadeCounter = 0;
        }

        // Apply fade value to both channels
        mixSampleLeft *= fadeValue;
        mixSampleRight *= fadeValue;
    }

    // --- Step 8: Final output assignment ---
    // Write the final stereo output sample to the provided references.
    left = mixSampleLeft;
    right = mixSampleRight;
}
