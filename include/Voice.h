#pragma once

#include "Oscillator.h"
#include "VoiceOptions.h"
#include "NoiseGenerator.h"
#include "SupersawOscillator.h"
#include "SineOscillator.h"
#include "SawOscillator.h"
#include "TriangleOscillator.h"
#include "SquareOscillator.h"
#include "MS20Filter.h"
#include "DSP.h"
#include "dsp_types.h"

// The PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// The Voice class manages two oscillators and handles interactions like
// Frequency Modulation (FM) and Oscillator Sync between them.
class Voice : public DSP
{
public:
    // Constructor: requires two Oscillator pointers and the global sample rateSample
    Voice();

    // Destructor: deletes both oscillator instances
    ~Voice();

    // Sets the type of FM to use
    void setFMType(FMType fm);

    // Sets the modulation index for frequency modulation.
    // This controls the intensity of the frequency modulation effect.
    void setFMModIndex(dsp_float index);

    // Enables or disables oscillator sync
    void setSyncEnabled(bool enabled);

    // Sets the duty cycle
    void setDutyCycle(dsp_float cycle);

    // Sets the pitch offset for the modulator
    void setPitchOffset(dsp_float offset);

    // Sets the fine tunig for the modulator
    void setFineTune(dsp_float fine);

    // Enables negative phase wrapping
    void setNegativeWrappingEnabled(bool enabled);

    // Sets the sample rate for signal calculation
    void setSampleRate(dsp_float rate);

    // Sets the size of the current audio buffer
    void setBlockSize(int size);

    // Sets the frequency of oscillator 1/carrier
    void setFrequency(dsp_float f);

    // Sets the volume level of the oscillators
    void setOscillatorMix(dsp_float mix);

    // Sets the volume level of the noise generator
    void setNoiseMix(dsp_float mix);

    // Assigns the carrier oscillator
    void setCarrierOscillatorType(CarrierOscillatiorType oscillatorType);

    // Assigns the modulator oscillator
    void setModulatorOscillatorType(ModulatorOscillatorType oscillatorType);

    // Changes the current noise type (white or pink)
    void setNoiseType(NoiseType type);

    // Sets the detune factor
    void setDetune(dsp_float value);

    // Sets the feedback amount for the carrier
    void setFeedbackCarrier(dsp_float feedback);

    // Sets the feedback amount for the modulator
    void setFeedbackModulator(dsp_float feedback);

    // Sets the filter type
    void setFilterMode(FilterMode mode);

    // Sets the cutoff frequency
    void setFilterCutoff(DSPBuffer *buffer);

    // Sets the filter resonance
    void setFilterResonance(DSPBuffer *buffer);

    // Sets the filter drive
    void setFilterDrive(dsp_float value);

    // Next sample block generation
    void computeSamples();

    DSPBuffer mixBufferL;  // Mixing buffer left channel
    DSPBuffer mixBufferR;  // Mixing buffer right channel

private:
    Oscillator *carrier;           // Carrier oscillator (may be modulated)
    Oscillator *modulator;         // Modulator oscillator (for FM or sync)
    Oscillator *carrierTmp;        // Carrier oscillator (may be modulated)
    Oscillator *modulatorTmp;      // Modulator oscillator (for FM or sync)
    bool applyOscillators = false; // Indicates that an oscillator has changed

    dsp_float frequency = 220.0; // Current frequency

    FMType fmType = FMType::ThroughZero; // The FM operation mode
    dsp_float modulationIndex = 0;          // FM depth: how much modulator modulates carrier

    dsp_float oscmix = 0.0;                 // Mix carrier <=> modulator
    dsp_float noisemix = 0.0;               // Mix oscillators <=> noise
    bool syncEnabled = false;            // True if Sync is active    
    bool negativeWrappingEnabled = true; // Indicates if negative phase wrapping is enabled
    
    dsp_float detune = 0;       // Detune factor supersaw oszillator
    dsp_float pulseWidth = 0.5; // Pulse width square oscillator
    dsp_float pitchOffset = 0;  // Pitch offset modulator
    dsp_float fineTune = 0;     // Fine tune modulator

    int fadeCounter = 0;       // Amplitude for param change
    const int fadeLength = 32; // Fade in/out samples for param change
    dsp_float fadeValue = 1.0;    // Current amplitude for param change

    // Oscillators
    NoiseGenerator *noise = new NoiseGenerator(); // Noise generator
    SupersawOscillator *supersawCarrier = new SupersawOscillator();
    SineOscillator *sineCarrier = new SineOscillator();
    SineOscillator *sineModulator = new SineOscillator();
    SawOscillator *sawCarrier = new SawOscillator();
    SawOscillator *sawModulator = new SawOscillator();
    SquareOscillator *squareCarrier = new SquareOscillator();
    SquareOscillator *squareModulator = new SquareOscillator();
    TriangleOscillator *trianlgeCarrier = new TriangleOscillator();
    TriangleOscillator *triangleModulator = new TriangleOscillator();
    
    // Multi mode filter
    MS20Filter *filter = new MS20Filter();

    // DSP working vars
    dsp_float carrierSampleLeft, carrierSampleRight;
    dsp_float modulatorSampleLeft, modulatorSampleRight;
    dsp_float noiseSampleLeft, noiseSampleRight;
    dsp_float mixSampleLeft, mixSampleRight;
    dsp_float amp_carrier;
    dsp_float amp_modulator;
    dsp_float amp_oscmix;
    dsp_float amp_osc_noise;
    dsp_float amp_noise;

    // Feedback
    dsp_float lastSampleCarrierLeft;
    dsp_float lastSampleCarrierRight;
    dsp_float lastSampleModulatorLeft;
    dsp_float lastSampleModulatorRight;
    dsp_float feedbackAmountCarrier = 0.0;
    dsp_float feedbackAmountModulator = 0.0;
};
