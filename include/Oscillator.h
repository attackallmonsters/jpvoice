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
    // The next mono audio sample as a floating point value in the range [-1.0, 1.0]
    virtual void getSample(double &left, double &right) = 0;

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
        frequency = calculatedFrequency = value;
    }

    // Sets the oscillator FM frequency in Hertz
    void setCalculatedFrequency(double value)
    {
        clampmin(value, 0.0);
        calculatedFrequency = value;
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
    double sampleRate;    // The audio systems current sampling rate
    double frequency;     // The desired oscillator frequency in Hertz
    double calculatedFrequency;   // The calculated FM frequency in Hertz
    double currentPhase;  // Current phase of the oscillator in radians [0, 2π]
    bool wrapped = false; // True when pahse wrapped
};
