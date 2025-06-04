#pragma once

#include <cmath>
#include "clamp.h"
#include "DSP.h"
#include "DSPObject.h"
#include "DSPBuffer.h"
#include "VoiceOptions.h"

// FM frequency calculation
using FMCalcFunc = double (*)(const double &baseFreq, const double &modulator, const double &index);

// Abstract base class for all oscillator types.
// This class provides a common interface and shared internal phase state
// for generating periodic waveforms based on frequency and sample rate.
// Concrete subclasses must implement their specific waveform behavior.
class Oscillator : public DSPObject
{
public:
    // Oscillator is initialized with current phase
    Oscillator();

    // Virtual destructor to allow proper cleanup in derived classes
    virtual ~Oscillator() = default;

    // Sets the detune factor
    virtual void setDetune(double /*value*/) {};

    // Sets the duty cycle for PWM
    virtual void setDutyCycle(double /*value*/) {};

    // Resets the internal oscillator phase to 0.0.
    virtual void resetPhase();

    // Sets the desired oscillator frequency in Hertz
    void setFrequency(double value);

    // Sets the pitch offset in semi tones
    void setPitchOffset(int value);

    // Sets the fine tuning in cent
    void setFineTune(double value);

    // Enables negative phase wrapping
    void setNegativeWrappingEnabled(bool enabled);

    // Calculates the effective frequency based on base frequency,
    // pitch offset (in semitones), and fine-tuning (in cents).
    // Then updates the phase increment accordingly.
    void setCalculatedFrequency(double f);

    // Gets the current frequency
    double getFrequency();

    // Gets the calculated FM frequency
    double getCalculatedFrequency();

    // Sets the type of FM to use
    void setFMType(FMType fm);

    // Sets the modulation index for frequency modulation.
    // This controls the intensity of the frequency modulation effect.
    void setFMModIndex(double index);

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    bool hasWrapped();

    // Resets the wrap status
    void unWrap();

    // Input buffer for FM, etc.
    DSPBuffer *inBufferL;
    DSPBuffer *inBufferR;

    // Input buffer for FM, etc.
    DSPBuffer outBufferL;
    DSPBuffer outBufferR;

protected:
    bool syncEnabled;                    // Enables or disable block wise phase synchronization
    bool negativeWrappingEnabled = true; // Indicates if negative phase wrapping is enabled
    double frequency;                    // The desired oscillator frequency in Hertz
    double calculatedFrequency;          // The calculated FM frequency in Hertz
    double pitchOffset;                  // offset in half tones
    double fineTune;                     // fine tune in cent
    double phaseIncrement;               // Increment based on frquency and sample rate
    double currentPhase;                 // Current phase of the oscillator in radians [0, 2π]
    bool wrapped = false;                // True when phase wrapped

    FMType fmType = FMType::ThroughZero; // The FM operation mode
    double modulationIndex = 0;          // FM depth: how much modulator modulates carrier
    FMCalcFunc fmFunc;                   // FM calculation method

    // Next sample block generation
    static void setSamplesIntern(DSPObject *dsp);

    // Avoid vtable lookup for sample calculation
    using ComputeSampleFunc = void (*)(Oscillator * /*osc*/, const double & /*phase*/, double & /*left*/, double & /*right*/);
    ComputeSampleFunc computeSampleFunc;

private:
    // Dummy ComputeSampleFunc for setSamples
    static void noopComputeSampleFunc(Oscillator * /*osc*/, const double & /*phase*/, double & /*left*/, double & /*right*/);
};
