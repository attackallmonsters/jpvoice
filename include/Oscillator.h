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
    //
    // Parameters:
    //   frequency   - Desired oscillator frequency in Hertz
    //   sampleRate  - The audio system's current sampling rate in samples per second
    //
    // Returns:
    //   The next mono audio sample as a floating point value in the range [-1.0, 1.0] (typically)
    virtual float getSample(float frequency, float sampleRate) = 0;

    // Resets the internal oscillator phase to 0.0.
    // This is useful when reinitializing the oscillator or synchronizing its phase.
    virtual void resetPhase() = 0;

    // Returns true if the oscillator's phase wrapped during the last getSample() call
    virtual bool hasWrapped() const = 0;

protected:
    // Current phase of the oscillator in radians.
    // Typically wraps within the range [0, 2π).
    double phase = 0.0;
};
