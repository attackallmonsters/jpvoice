#pragma once

#include <cmath>
#include "clamp.h"

// Abstract base class for all oscillator types.
// This class provides a common interface and shared internal phase state
// for generating periodic waveforms based on frequency and sample rate.
// Concrete subclasses must implement their specific waveform behavior.
class Oscillator
{
public:
    // Oscillator is initialized with current phase
    Oscillator() = default;

    // Virtual destructor to allow proper cleanup in derived classes
    virtual ~Oscillator() = default;

    // Generates the next audio sample for the oscillator.
    // This function must be implemented by each specific oscillator type (e.g., sine, saw, square).
    // Returns:
    // The next mono audio sample as a floating point value in the range [-1.0, 1.0]
    virtual void getSample(double &left, double &right) = 0;

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

    // Sets the audio systems current sampling rate
    void setSampleRate(double value)
    {
        clampmin(value, 0.0);
        sampleRate = value;
        setFrequency(frequency);
    }

    // Gets the current sampling rate
    double getSampeRate()
    {
        return sampleRate;
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

    // Calculates the effective frequency based on base frequency,
    // pitch offset (in semitones), and fine-tuning (in cents).
    // Then updates the phase increment accordingly.
    void setCalculatedFrequency(double value)
    {
        double f = clampmin(value, 0.0);

        // Convert pitch offset and fine tune to a total semitone offset
        double semitoneOffset = static_cast<double>(pitchOffset) + (fineTune / 100.0);

        // Apply equal temperament formula: f = f0 * 2^(n/12)
        calculatedFrequency = f * std::pow(2.0, semitoneOffset / 12.0);

        // Update phase increment for waveform generation
        phaseIncrement = calculatedFrequency / sampleRate;
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

protected:
    double sampleRate;          // The audio systems current sampling rate
    double frequency;           // The desired oscillator frequency in Hertz
    double calculatedFrequency; // The calculated FM frequency in Hertz
    double pitchOffset;         // offset in half tones
    double fineTune;            // fine tune in cent
    double phaseIncrement;      // Increment based on frquency and sample rate
    double currentPhase;        // Current phase of the oscillator in radians [0, 2π]
    bool wrapped = false;       // True when pahse wrapped
    
};
