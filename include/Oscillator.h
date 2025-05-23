#pragma once

// Abstract base class for all oscillator types.
// This class provides a common interface and shared internal phase state
// for generating periodic waveforms based on frequency and sample rate.
// Concrete subclasses must implement their specific waveform behavior.
class Oscillator
{
public:
    // Virtual destructor to allow proper cleanup in derived classes
    virtual ~Oscillator() = default;

    // Generates the next audio sample for the oscillator.
    // This function must be implemented by each specific oscillator type (e.g., sine, saw, square).
    // Returns:
    //   The next mono audio sample as a floating point value in the range [-1.0, 1.0]
    virtual double getSample() = 0;

    // Resets the internal oscillator phase to 0.0.
    // This is useful when reinitializing the oscillator or synchronizing its phase.
    virtual void resetPhase() = 0;

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    virtual bool hasWrapped() const = 0;

    // Sets the value for supersaw detuning
    void setDetune(double value)
    {
        if (value < 0.0)
            value = 0.0;

        if (value > 1.0)
            value = 1.0;

        detune = value;
    }

    // Sets the audio systems current sampling rate
    void setSampleRate(double value)
    {
        if (value < 0.0)
            value = 0.0;

        sampleRate = value;
    }

    // Gets the current sampling rate
    double getSampeRate() { return sampleRate; }

    // Sets the desired oscillator frequency in Hertz
    void setFrequency(double value)
    {
        if (value < 0.0)
            value = 0.0;

        frequency = value;
    }

    // Gets teh current frequency
    double getFrequency() { return frequency; }

protected:
    // The audio systems current sampling rate
    double sampleRate;
    // The desired oscillator frequency in Hertz
    double frequency;
    // Current phase of the oscillator in radians.
    // Typically wraps within the range [0, 2π).
    double phase = 0.0;

    // Detune for supersaw
    double detune = 0.0;
};
