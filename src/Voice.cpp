#include <cmath>
#include "Voice.h"
#include "clamp.h"
#include "VoiceOptions.h"
#include "dsp_util.h"

// Constructor: initializes the voice with two oscillator instances.
// These oscillators are externally allocated and represent the carrier (carrier) and modulator (modulator).
Voice::Voice()
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
    DSP::setSampleRate(rate);
}

// Sets the size of the current audio buffer
void Voice::setBlockSize(int size)
{
    DSP::setBlockSize(size);
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
void Voice::setFilterMode(FilterMode /*mode*/)
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

// Sets the filter stage
void Voice::setFilterStage(FilterStage stage)
{
    filter->setFilterStage(stage);
}

// Next sample block generation
void Voice::computeSamples()
{
    // --- Step 1: Get stereo signal from the modulator oscillator ---
    // This signal will be used either for frequency modulation or direct audio mixing.
    modulator->setSamples();

    // --- Step 2: Apply Through Zero Frequency Modulation if enabled ---
    // Use the average (mono) value of the stereo modulator signal
    // to modulate the frequency of the carrier oscillator.
    // if (modulationIndex > 0)
    // {
    //     double frequencyCarrier;
    //     double modulatorSample = 0.5 * (modulatorSampleLeft + modulatorSampleRight); // Convert to mono

    //     switch (fmType)
    //     {
    //     case FMType::Linear:
    //         frequencyCarrier = frequency + modulatorSample * modulationIndex;
    //         break;

    //     case FMType::Relative:
    //         frequencyCarrier = frequency + modulatorSample * modulationIndex * frequency;
    //         break;

    //     case FMType::ThroughZero:
    //         frequencyCarrier = frequency + modulatorSample * modulationIndex;
    //         break;
    //     default:
    //         frequencyCarrier = frequency;
    //         break;
    //     }

    //     carrier->setCalculatedFrequency(frequencyCarrier); // Set the modulated frequency
    // }

    // --- Step 3: Generate the stereo output from the carrier oscillator ---
    // This will either be used directly (in FM mode) or mixed with the modulator signal.
    carrier->setSamples();

    // --- Step 4: Sync handling ---
    // If sync is enabled and the carrier oscillator has wrapped (phase reset),
    // we reset the phase of the modulator oscillator to align it.
    if (syncEnabled && carrier->hasWrapped())
    {
        modulator->resetPhase();
        carrier->unWrap();
    }

    if (noisemix > 0)
    {
        noise->setSamples();
    }

    // --- Step 5: Mix the carrier, modulator, feedback and noise signals using an equal-power crossfade
    amp_carrier = std::cos(oscmix * 0.5 * M_PI);
    amp_modulator = std::sin(oscmix * 0.5 * M_PI);
    amp_osc_noise = std::cos(noisemix * 0.5 * M_PI);
    amp_noise = std::sin(noisemix * 0.5 * M_PI);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        double carrierLeft = carrier->outBufferL[i];
        double carrierRight = carrier->outBufferR[i];
        double modLeft = modulator->outBufferL[i];
        double modRight = modulator->outBufferR[i];

        // Mix mit Feedback
        double mixL = amp_carrier * (carrierLeft + lastSampleCarrierLeft) + amp_modulator * (modLeft + lastSampleModulatorLeft);
        double mixR = amp_carrier * (carrierRight + lastSampleCarrierRight) + amp_modulator * (modRight + lastSampleModulatorRight);

        lastSampleCarrierLeft = carrierLeft * feedbackAmountCarrier;
        lastSampleCarrierRight = carrierRight * feedbackAmountCarrier;
        lastSampleModulatorLeft = modLeft * feedbackAmountModulator;
        lastSampleModulatorRight = modRight * feedbackAmountModulator;

        // Noise (optional)
        if (noisemix > 0)
        {
            mixL = amp_osc_noise * mixL + amp_noise * noise->outBufferL[i];
            mixR = amp_osc_noise * mixR + amp_noise * noise->outBufferR[i];
        }

        // Clipping
        mixBufferL[i] = fast_tanh(mixL);
        mixBufferR[i] = fast_tanh(mixR);
    }

    // Step 6: assign buffers to ladder filter
    // filter->bufferL = &mixBufferL;
    // filter->bufferR = &mixBufferR;

    // Calculate the samples to be filtered
    //filter->setSamples();

    // --- Step 7: Smooth fade-out/fade-in when parameters change ---
    if (applyOscillators)
    {
        fadeCounter++;

        if (fadeCounter <= fadeLength)
        {
            fadeValue = 1.0 - (double(fadeCounter) / fadeLength); // Fade out
        }
        else if (fadeCounter == fadeLength + 1)
        {
            if (carrier != carrierTmp)
                carrier = carrierTmp;

            if (modulator != modulatorTmp)
                modulator = modulatorTmp;
        }
        else if (fadeCounter <= fadeLength * 2)
        {
            fadeValue = (double(fadeCounter - fadeLength) / fadeLength); // Fade in
        }
        else
        {
            applyOscillators = false;
            fadeValue = 1.0;
            fadeCounter = 0;
        }

        // Apply fade to entire output buffer
        for (size_t i = 0; i < DSP::blockSize; ++i)
        {
            mixBufferL[i] *= fadeValue;
            mixBufferR[i] *= fadeValue;
        }
    }
}
