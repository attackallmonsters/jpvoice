// === ms20filter.h ===
#pragma once

#include "DSPObject.h"
#include "DSPBuffer.h"

class MS20Filter : public DSPObject
{
public:
    // Constructor with sample rate
    explicit MS20Filter();

    // Set cutoff frequency in Hz
    void setCutoff(DSPBuffer *buffer);

    // Set resonance amount (typically 0.0 to ~1.0)
    void setResonance(DSPBuffer *buffer);

    // Assigns the samples to process
    void setSampleBuffers(DSPBuffer *samplesL, DSPBuffer *samplesR);

    // Reset internal filter state
    void reset();

private:
    // Filter state variables:
    double y1L; // Output of first integrator left
    double y2L; // Output of second integrator (filter output) left
    double y1R; // Output of first integrator right
    double y2R; // Output of second integrator (filter output) right
    double T;   // Simplified impulse invariant/bilinear transformation
    static double nonlinearFeedback(double s); // Nonlinear feedback (simulates diode behavior)

    // Processes data in bufferL, buffer R
    static void processBlock(DSPObject *dsp);

    // The samples to be filtered
    DSPBuffer *bufferL;
    DSPBuffer *bufferR;

    // Control buffer for cutoff
    DSPBuffer *cutoffBuffer;

    // Control buffer for resonance
    DSPBuffer *resoBuffer;
};
