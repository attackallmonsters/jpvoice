#pragma once

class DSPBase
{
public:
    // Generates the next audio sample for the oscillator.
    // This function must be implemented by each specific oscillator type (e.g., sine, saw, square).
    // Returns:
    // The next mono audio sample as a floating point value in the range [-1.0, 1.0]
    void getSample(double &left, double &right)
    {
        (*sampleFunc)(this, left, right);
    }

    // avoid vtable lookup
    using SampleFunc = void (*)(DSPBase *, double &, double &);
    SampleFunc sampleFunc; 
};