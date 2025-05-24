#pragma once

#include "tool.h"

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
    //   The next mono audio sample as a floating point value in the range [-1.0, 1.0]
    virtual double getSample() = 0;

    // Sets the detune factor
    virtual void setDetune(double /*value*/) {};

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
    }

    // Gets the current sampling rate
    double getSampeRate()
    {
        return sampleRate;
    }

    // Sets the desired oscillator frequency in Hertz
    void setFrequency(double value)
    {
        clampmin(value, 0.0);
        frequency = value;
    }

    // Gets the current frequency
    double getFrequency()
    {
        return frequency;
    }

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    bool hasWrapped()
    {
        return wrapped;
    }

protected:
    // The audio systems current sampling rate
    double sampleRate;
    // The desired oscillator frequency in Hertz
    double frequency;
    // Current phase of the oscillator in radians.
    // Typically wraps within the range [0, 2π).
    double currentPhase;
    // True when pahse wrapped
    bool wrapped = false;
};
