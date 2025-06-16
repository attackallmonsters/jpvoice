// === ms20filter.h ===
#pragma once

#include "DSPObject.h"
#include "DSPBuffer.h"
#include "dsp_types.h"

class KorgonFilter : public DSPObject
{
public:
    // Constructor with sample rate
    explicit KorgonFilter();

    // Initializes the filter
    void Initialize() override;

    // Sets the filter drive
    void setDrive(dsp_float value);

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
    dsp_float y1L;   // Output of first integrator left
    dsp_float y2L;   // Output of second integrator (filter output) left
    dsp_float y1R;   // Output of first integrator right
    dsp_float y2R;   // Output of second integrator (filter output) right
    dsp_float T;     // Simplified impulse invariant/bilinear transformation
    dsp_float drive; // The filter drive

    static dsp_float nonlinearFeedback(dsp_float s); // Nonlinear feedback (simulates diode behavior)

    // Processes data in bufferL, buffer R
    static void processBlock(DSPObject *dsp);

    // The samples to be filtered
    DSPBuffer *bufferL;
    DSPBuffer *bufferR;

    // Control buffer for cutoff
    DSPBuffer *cutoffBuffer;
    DSPBuffer cutoffInitBuffer;

    // Control buffer for resonance
    DSPBuffer *resoBuffer;
    DSPBuffer resoInitBuffer;
};
