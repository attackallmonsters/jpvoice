#include <cmath>
#include "Voice.h"
#include "clamp.h"
#include "VoiceOptions.h"
#include "dsp_util.h"
#include "dsp_types.h"

int Voice::voiceCounter = 0;

// Constructor: initializes the voice with two oscillator instances.
// These oscillators are externally allocated and represent the carrier (carrier) and modulator (modulator).
Voice::Voice()
{
    carrier = sawCarrier;
    modulator = sineModulator;
}

// Destructor: cleans up oscillator instances.
// It is assumed that carrier and modulator were allocated using 'new' and are not shared elsewhere.
Voice::~Voice()
{
    delete noise;
    delete sineCarrier;
    delete sineModulator;
    delete sawCarrier;
    delete sawModulator;
    delete squareCarrier;
    delete squareModulator;
    delete trianlgeCarrier;
    delete triangleModulator;
}

void Voice::Initialize()
{
    DSP::Initialize();

    if (!DSP::dspIsInitialized())
    {
        DSP::log("DSP not initialized. Do DSP::initializeAudio first.");
        throw("DSP not initialized. Do DSP::initializeAudio first.");
    }

    voiceNumber = ++Voice::voiceCounter;

    DSP::log("Initializing jpvoice... %i", voiceNumber);

    // Waveform generation
    sawCarrier->Initialize();
    sawModulator->Initialize();
    sineCarrier->Initialize();
    sineModulator->Initialize();
    squareCarrier->Initialize();
    squareModulator->Initialize();
    trianlgeCarrier->Initialize();
    triangleModulator->Initialize();

    mixBufferL.resize(DSP::blockSize);
    mixBufferR.resize(DSP::blockSize);

    DSP::log("jpvoice... %i initialized", voiceNumber);
}

// Sets the type of FM to use
void Voice::setFMType(FMType fm)
{
    fmType = fm;
    carrier->setFMType(fmType);
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void Voice::setFMModIndex(dsp_float index)
{
    modulationIndex = index;
    carrier->setFMModIndex(modulationIndex);
}

// Enables or disables oscillator synchronization.
// When enabled and carrier wraps its phase, modulator will be reset to phase 0.
void Voice::setSyncEnabled(bool enabled)
{
    syncEnabled = enabled;
}

// Sets the duty cycle
void Voice::setDutyCycle(dsp_float cycle)
{
    dsp_float dutyCycle = clamp(cycle, 0.0, 1.0);
    squareModulator->setDutyCycle(dutyCycle);
    squareCarrier->setDutyCycle(dutyCycle);
}

// Sets the pitch offset for the modulator
void Voice::setPitchOffset(dsp_float offset)
{
    pitchOffset = clamp(offset, -24.0, 24.0);
    modulator->setPitchOffset(pitchOffset);
}

// Sets the fine tunig for the modulator
void Voice::setFineTune(dsp_float fine)
{
    fineTune = clamp(fine, -100.0, 100.0);
    modulator->setFineTune(fineTune);
}

// Enables negative phase wrapping
void Voice::setNegativeWrappingEnabled(bool enabled)
{
    negativeWrappingEnabled = enabled;
}

// Sets the current frequency
void Voice::setFrequency(dsp_float f)
{
    carrier->setFrequency(f);
    modulator->setFrequency(f);
    frequency = f;
}

// Sets the detune factorjpvoice_tilde_sync
void Voice::setDetune(dsp_float value)
{
    detune = value;
    carrier->setDetune(detune);
}

// Sets the number of voices
void Voice::setNumVoices(int count)
{
    if (count == numVoices)
        return;

    numVoices = count;
    carrier->setNumVoices(numVoices);
}

// Sets the volume level of the oscillators
void Voice::setOscillatorMix(dsp_float mix)
{
    oscmix = clamp(mix, 0.0, 1.0);
}

// Sets the volume level of the noise generator
void Voice::setNoiseMix(dsp_float mix)
{
    noisemix = clamp(mix, 0.0, 1.0);
}

// Assigns the carrier oscillator
void Voice::setCarrierOscillatorType(CarrierOscillatiorType oscillatorType)
{
    dsp_float f = (carrier) ? carrier->getFrequency() : 0.0;

    switch (oscillatorType)
    {
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
        carrierTmp = sawCarrier;
        break;
    }

    if (carrierTmp == carrier)
    {
        return;
    }

    carrierTmp->setFrequency(f);
    carrierTmp->setNegativeWrappingEnabled(negativeWrappingEnabled);
    carrierTmp->setFMType(fmType);
    carrierTmp->setFMModIndex(modulationIndex);
    carrierTmp->setDetune(detune);
    carrierTmp->setNumVoices(numVoices);

    applyOscillators = true;
}

// Assigns the modulation oscillator
void Voice::setModulatorOscillatorType(ModulatorOscillatorType oscillatorType)
{
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

    modulatorTmp->setFrequency(frequency);
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
void Voice::setFeedbackCarrier(dsp_float feedback)
{
    feedbackAmountCarrier = clamp(feedback, 0.0, 2.0);
}

// Sets the feedback amount for the modulator
void Voice::setFeedbackModulator(dsp_float feedback)
{
    feedbackAmountModulator = clamp(feedback, 0.0, 2.0);
}

// Sets the filter type
void Voice::setFilterMode(FilterMode /*mode*/)
{
    // TODO
}

// Sets the cutoff frequency
void Voice::setFilterCutoff(DSPBuffer *buffer)
{
    filter->setCutoff(buffer);
}

// Sets the filter resonance
void Voice::setFilterResonance(DSPBuffer *buffer)
{
    filter->setResonance(buffer);
}

// Sets the filter drive
void Voice::setFilterDrive(dsp_float value)
{
    filter->setDrive(value);
}

// Next sample block generation
void Voice::computeSamples()
{
    // --- Step 1: Get stereo signal from the modulator oscillator ---
    // This signal will be used either for frequency modulation or direct audio mixing.
    modulator->generateBlock();

    // --- Step 2: Apply Through Zero Frequency Modulation if enabled ---
    // Use the average (mono) value of the stereo modulator signal
    // to modulate the frequency of the carrier oscillator.
    if (modulationIndex > 0)
    {
        // TODO: Only assign once because from then on the buffers are identical
        // If this is done on oscillator switch, modBufferL is invalid (and modBufferR valid)
        carrier->modBufferL = &modulator->outBufferL;
        carrier->modBufferR = &modulator->outBufferR;
    }

    // --- Step 3: Generate the stereo output from the carrier oscillator ---
    // This will either be used directly (in FM mode) or mixed with the modulator signal.
    carrier->generateBlock();

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
        noise->generateBlock();
    }

    // --- Step 5: Mix the carrier, modulator, feedback and noise signals using an equal-power crossfade
    amp_carrier = std::cos(oscmix * 0.5 * M_PI);
    amp_modulator = std::sin(oscmix * 0.5 * M_PI);
    amp_osc_noise = std::cos(noisemix * 0.5 * M_PI);
    amp_noise = std::sin(noisemix * 0.5 * M_PI);

    for (size_t i = 0; i < DSP::blockSize; ++i)
    {
        carrierLeft = carrier->outBufferL[i] + lastSampleCarrierLeft * feedbackAmountCarrier;
        carrierRight = carrier->outBufferR[i] + lastSampleCarrierRight * feedbackAmountCarrier;
        modLeft = modulator->outBufferL[i] + lastSampleModulatorLeft * feedbackAmountModulator;
        modRight = modulator->outBufferR[i] + lastSampleModulatorRight * feedbackAmountModulator;

        mixL = amp_carrier * carrierLeft + amp_modulator * modLeft;
        mixR = amp_carrier * carrierRight + amp_modulator * modRight;

        if (feedbackAmountCarrier > 0)
        {
            lastSampleCarrierLeft = fast_tanh(carrierLeft);
            lastSampleCarrierRight = fast_tanh(carrierRight);
        }

        if (feedbackAmountModulator > 0)
        {
            lastSampleModulatorLeft = fast_tanh(modLeft);
            lastSampleModulatorRight = fast_tanh(modRight);
        }

        // // Noise (optional)
        if (noisemix > 0)
        {
            mixL = amp_osc_noise * mixL + amp_noise * noise->outBufferL[i];
            mixR = amp_osc_noise * mixR + amp_noise * noise->outBufferR[i];
        }

        // Clipping
        mixBufferL[i] = mixL;
        mixBufferR[i] = mixR;
    }

    // Step 6: assign buffers to ladder filter
    filter->setSampleBuffers(&mixBufferL, &mixBufferR);

    // Calculate the samples to be filtered
    filter->generateBlock();

    // --- Step 7: Smooth fade-out/fade-in when parameters change ---
    if (applyOscillators)
    {
        fadeCounter++;

        if (fadeCounter <= fadeLength)
        {
            fadeValue = 1.0 - (dsp_float(fadeCounter) / fadeLength); // Fade out
        }
        else if (fadeCounter == fadeLength + 1)
        {
            if (carrier != carrierTmp)
                carrier = carrierTmp;

            if (modulator != modulatorTmp)
                modulator = modulatorTmp;

            filter->reset();
        }
        else if (fadeCounter <= fadeLength * 2)
        {
            fadeValue = (dsp_float(fadeCounter - fadeLength) / fadeLength); // Fade in
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
