#pragma once

#include "Oscillator.h"
#include "VoiceOptions.h"
#include "NoiseGenerator.h"
#include "SupersawOscillator.h"
#include "SineOscillator.h"
#include "SawOscillator.h"
#include "TriangleOscillator.h"
#include "SquareOscillator.h"
#include "LadderFilter.h"
#include "DSP.h"

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

    // Enables or disables oscillator sync
    void setSyncEnabled(bool enabled);

    // Sets the duty cycle
    void setDutyCycle(double cycle);

    // Sets the pitch offset for the modulator
    void setPitchOffset(double offset);

    // Sets the fine tunig for the modulator
    void setFineTune(double fine);

    // Sets the type of FM to use
    void setFMType(FMType fm);

    // Sets the modulation index used when FM is active
    void setFMModIndex(double index);

    // Enables negative phase wrapping
    void setNegativeWrappingEnabled(bool enabled);

    // Sets the sample rate for signal calculation
    void setSampleRate(double rate);

    // Sets the size of the current audio buffer
    void setBlockSize(int size);

    // Sets the frequency of oscillator 1/carrier
    void setFrequency(double f);

    // Sets the volume level of the oscillators
    void setOscillatorMix(double mix);

    // Sets the volume level of the noise generator
    void setNoiseMix(double mix);

    // Assigns the carrier oscillator
    void setCarrierOscillatorType(CarrierOscillatiorType oscillatorType);

    // Assigns the modulator oscillator
    void setModulatorOscillatorType(ModulatorOscillatorType oscillatorType);

    // Changes the current noise type (white or pink)
    void setNoiseType(NoiseType type);

    // Sets the detune factor
    void setDetune(double value);

    // Sets the feedback amount for the carrier
    void setFeedbackCarrier(double feedback);

    // Sets the feedback amount for the modulator
    void setFeedbackModulator(double feedback);

    // Sets the filter type
    void setFilterMode(FilterMode mode);

    // Sets the cutoff frequency
    void setCutoffFrequency(double frequency);

    // Sets the filter resonance
    void setResonance(double value);

    // Sets the filter drive
    void setDrive(double value);

    // Sets the filter stage
    void setFilterStage(FilterStage stage);

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

    double frequency = 220.0; // Current frequency

    double oscmix = 0.0;                 // Mix carrier <=> modulator
    double noisemix = 0.0;               // Mix oscillators <=> noise
    bool syncEnabled = false;            // True if Sync is active
    FMType fmType = FMType::ThroughZero; // The FM operation mode
    bool negativeWrappingEnabled = true; // Indicates if negative phase wrapping is enabled
    double modulationIndex = 0;          // FM depth: how much modulator modulates carrier

    double detune = 0;       // Detune factor supersaw oszillator
    double pulseWidth = 0.5; // Pulse width square oscillator
    double pitchOffset = 0;  // Pitch offset modulator
    double fineTune = 0;     // Fine tune modulator

    int fadeCounter = 0;       // Amplitude for param change
    const int fadeLength = 32; // Fade in/out samples for param change
    double fadeValue = 1.0;    // Current amplitude for param change

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
    LadderFilter *filter = new LadderFilter();

    // DSP working vars
    double carrierSampleLeft, carrierSampleRight;
    double modulatorSampleLeft, modulatorSampleRight;
    double noiseSampleLeft, noiseSampleRight;
    double mixSampleLeft, mixSampleRight;
    double amp_carrier;
    double amp_modulator;
    double amp_oscmix;
    double amp_osc_noise;
    double amp_noise;

    // Feedback
    double lastSampleCarrierLeft;
    double lastSampleCarrierRight;
    double lastSampleModulatorLeft;
    double lastSampleModulatorRight;
    double feedbackAmountCarrier = 0.0;
    double feedbackAmountModulator = 0.0;
};
