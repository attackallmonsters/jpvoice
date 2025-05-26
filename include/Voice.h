#pragma once

#include "Oscillator.h"
#include "OscillatorOptions.h"
#include "NoiseGenerator.h"
#include "SupersawOscillator.h"
#include "SineOscillator.h"
#include "SawOscillator.h"
#include "TriangleOscillator.h"
#include "SquareOscillator.h"

// The PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// The Voice class manages two oscillators and handles interactions like
// Frequency Modulation (FM) and Oscillator Sync between them.
class Voice
{
public:
    // Constructor: requires two Oscillator pointers and the global sample rateSample
    Voice();

    // Destructor: deletes both oscillator instances
    ~Voice();

    // Enables or disables frequency modulation (FM)
    void setFMEnabled(bool enabled);

    // Enables or disables oscillator sync
    void setSyncEnabled(bool enabled);

    // Sets the modulation index used when FM is active
    void setModIndex(double index);

    // Sets teh sample rate for signal calculation
    void setSampleRate(double sampleRate);

    // Sets the frequency of oscillator 1/carrier
    void setFrequencyCarrier(double frequency);

    // Sets the frequency of oscillator 2/modulator
    void setFrequencyModulator(double frequency);

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

    // Computes and returns one audio sample by combining both oscillators at a given sample rate
    void getSample(double &left, double &right);

    // Applies changed paramters on low amplitude
    void applyChangedParameters();

private:
    Oscillator *carrier;               // Carrier oscillator (may be modulated)
    Oscillator *modulator;             // Modulator oscillator (for FM or sync)
    double frequencyCarrier = 220.0;   // Current frequency carrier
    double frequencyModulator = 220.0; // Current frequency modulator

    double oscmix = 0.0;          // Mix carrier <=> modulator
    double noisemix = 0.0;        // Mix oscillators <=> noise
    bool fmEnabled = false;       // True if FM is active
    bool syncEnabled = false;     // True if Sync is active
    double modulationIndex = 1.0; // FM depth: how much modulator modulates carrier

    double detune = 0;       // Detune factor supersaw oszillator
    double pulseWidth = 0.5; // Pulse width square oscillator

    bool paramChanged = false; // Indicates a paramter change
    int fadeCounter = 0;       // Amplitude for param change
    const int fadeLength = 32; // Fade in/out samples for param change
    double fadeValue = 1.0;    // Current amplitude for param change

    // Oscillators
    NoiseGenerator *noise = new NoiseGenerator(); // Noise generator
    SupersawOscillator *supersawCarrier = new SupersawOscillator();
    SineOscillator *sineModulator = new SineOscillator();
    SawOscillator *sawCarrier = new SawOscillator();
    SawOscillator *sawModulator = new SawOscillator();
    SquareOscillator *squareCarrier = new SquareOscillator();
    SquareOscillator *squareModulator = new SquareOscillator();
    TriangleOscillator *trianlgeCarrier = new TriangleOscillator();
    TriangleOscillator *triangleModulator = new TriangleOscillator();

    // DSP working vars
    double carrierSampleLeft, carrierSampleRight;
    double modulatorSampleLeft, modulatorSampleRight;
    double noiseSampleLeft, noiseSampleRight;
    double mixSampleLeft, mixSampleRight;
    double amp_carrier;
    double amp_modulator;
    double amp_oscmix;
    double amp_noise;

};
