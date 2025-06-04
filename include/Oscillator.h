#pragma once

#include <cmath>
#include "clamp.h"
#include "DSP.h"
#include "DSPObject.h"
#include "DSPBuffer.h"

// Abstract base class for all oscillator types.
// This class provides a common interface and shared internal phase state
// for generating periodic waveforms based on frequency and sample rate.
// Concrete subclasses must implement their specific waveform behavior.
class Oscillator : public DSPObject
{
public:
    // Oscillator is initialized with current phase
    Oscillator() = default;

    // Virtual destructor to allow proper cleanup in derived classes
    virtual ~Oscillator() = default;

    // Sets the detune factor
    virtual void setDetune(double /*value*/) {};

    // Sets the duty cycle for PWM
    virtual void setDutyCycle(double /*value*/) {};

    // Resets the internal oscillator phase to 0.0.
    virtual void resetPhase()
    {
        currentPhase = 0.0;
        wrapped = false;
    }

    // Sets the desired oscillator frequency in Hertz
    void setFrequency(double value)
    {
        frequency = clampmin(value, 0.0);
        setCalculatedFrequency(frequency);
    }

    // Sets the pitch offset in semi tones
    void setPitchOffset(int value)
    {
        pitchOffset = clamp(value, -24, 24);
        setCalculatedFrequency(frequency);
    }

    // Sets the fine tuning in cent
    void setFineTune(double value)
    {
        fineTune = clamp(value, -100.0, 100.0);
        setCalculatedFrequency(frequency);
    }

    // Enables negative phase wrapping
    void setNegativeWrappingEnabled(bool enabled)
    {
        negativeWrappingEnabled = enabled;
    }

    // Calculates the effective frequency based on base frequency,
    // pitch offset (in semitones), and fine-tuning (in cents).
    // Then updates the phase increment accordingly.
    void setCalculatedFrequency(double f)
    {
        // Convert pitch offset and fine tune to a total semitone offset
        double semitoneOffset = static_cast<double>(pitchOffset) + (fineTune / 100.0);

        // Apply equal temperament formula: f = f0 * 2^(n/12)
        calculatedFrequency = f * std::pow(2.0, semitoneOffset / 12.0);

        // Update phase increment for waveform generation
        phaseIncrement = calculatedFrequency / DSP::sampleRate;
    }

    // Gets the current frequency
    double getFrequency()
    {
        return frequency;
    }

    // Gets the calculated FM frequency
    double getCalculatedFrequency()
    {
        return calculatedFrequency;
    }

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    bool hasWrapped()
    {
        return wrapped;
    }

    // Resets the wrap status
    void unWrap()
    {
        wrapped = false;
    }

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
};
