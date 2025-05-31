#include <cmath>
#include "Voice.h"
#include "clamp.h"
#include "VoiceOptions.h"

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

// Sets the type of FM to use
void Voice::setFMType(FMType fm)
{
    fmType = fm;
    negativeWrappingEnabled = fmType == FMType::ThroughZero;
    carrier->setNegativeWrappingEnabled(negativeWrappingEnabled);

    if (fmType == FMType::Relative && modulationIndex > 30.0)
    {
        setFMModIndex(30);
    }
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void Voice::setFMModIndex(double index)
{
    double modmax = (fmType == FMType::Relative) ? 30 : 1000;
    modulationIndex = clamp(index, 0.0, modmax);

    if (modulationIndex == 0)
    {
        carrier->setFrequency(frequency);
        return;
    }
}

// Enables negative phase wrapping
void Voice::setNegativeWrappingEnabled(bool enabled)
{
    negativeWrappingEnabled = enabled;
}

// Sets teh sample rate for signal calculation
void Voice::setSampleRate(double rate)
{
    sampleRate = clamp(rate, 44100.0, 96000.0);

    noise->setSampleRate(sampleRate);
    supersawCarrier->setSampleRate(sampleRate);
    sineCarrier->setSampleRate(sampleRate);
    sineModulator->setSampleRate(sampleRate);
    sawCarrier->setSampleRate(sampleRate);
    sawModulator->setSampleRate(sampleRate);
    squareCarrier->setSampleRate(sampleRate);
    squareModulator->setSampleRate(sampleRate);
    trianlgeCarrier->setSampleRate(sampleRate);
    triangleModulator->setSampleRate(sampleRate);
    filter->setSampleRate(sampleRate);
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
    carrier->setNegativeWrappingEnabled(negativeWrappingEnabled);
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

// Sets the feedback amount for the carrier
void Voice::setFeedbackCarrier(double feedback)
{
    feedbackAmountCarrier = clamp(feedback, 0.0, 2.0);
}

// Sets the feedback amount for the modulator
void Voice::setFeedbackModulator(double feedback)
{
    feedbackAmountModulator = clamp(feedback, 0.0, 2.0);
}

// Sets the filter type
void Voice::setFilterMode(FilterMode mode)
{
    // TODO
}

// Sets the cutoff frequency
void Voice::setCutoffFrequency(double frequency)
{
    filter->setCutoff(frequency);
}

// Sets the filter resonance
void Voice::setResonance(double value)
{
    filter->setResonance(value);
}

// Sets the filter drive
void Voice::setDrive(double value)
{
    filter->setDrive(value);
}

// Computes and returns a single audio sample from the voice.
// Takes two input frequencies (for carrier and modulator) and the current sample rate.
// Depending on the modulation settings, the sample result is computed differently.
void Voice::getSample(double &left, double &right)
{
    // --- Step 1: Get stereo signal from the modulator oscillator ---
    // This signal will be used either for frequency modulation or direct audio mixing.
    modulator->getSample(modulatorSampleLeft, modulatorSampleRight);

    // --- Step 2: Apply Through Zero Frequency Modulation if enabled ---
    // Use the average (mono) value of the stereo modulator signal
    // to modulate the frequency of the carrier oscillator.
    if (modulationIndex > 0)
    {
        double frequencyCarrier;
        double modulatorSample = 0.5 * (modulatorSampleLeft + modulatorSampleRight); // Convert to mono

        switch (fmType)
        {
        case FMType::Linear:
            frequencyCarrier = frequency + modulatorSample * modulationIndex;
            break;

        case FMType::Relative:
            frequencyCarrier = frequency + modulatorSample * modulationIndex * frequency;
            break;

        case FMType::ThroughZero:
            frequencyCarrier = frequency + modulatorSample * modulationIndex;
            break;
        default:
            frequencyCarrier = frequency;
            break;
        }

        carrier->setCalculatedFrequency(frequencyCarrier); // Set the modulated frequency
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

    // --- Step 5.2: Mix the carrier and modulator signals using an equal-power crossfade.
    // The oscmix parameter defines the mix balance (0.0 = only carrier, 1.0 = only modulator).
    amp_carrier = std::cos(oscmix * 0.5 * M_PI);
    amp_modulator = std::sin(oscmix * 0.5 * M_PI);

    mixSampleLeft = amp_carrier * (carrierSampleLeft + lastSampleCarrierLeft) + amp_modulator * (modulatorSampleLeft + lastSampleModulatorLeft);
    mixSampleRight = amp_carrier * (carrierSampleRight + lastSampleCarrierRight) + amp_modulator * (modulatorSampleRight + lastSampleModulatorRight);

    lastSampleCarrierLeft = carrierSampleLeft * feedbackAmountCarrier;
    lastSampleCarrierRight = carrierSampleRight * feedbackAmountCarrier;
    lastSampleModulatorLeft = modulatorSampleLeft * feedbackAmountModulator;
    lastSampleModulatorRight = modulatorSampleRight * feedbackAmountModulator;

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

    // Filter currently not working
    filter->getSample(mixSampleLeft, mixSampleRight);

    // --- Step 8: Final output assignment ---
    // Clip & write the final stereo output sample to the provided references.
    left = std::tanh(mixSampleLeft);
    right = std::tanh(mixSampleRight);
}
