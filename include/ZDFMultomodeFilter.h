#pragma once
#include <cmath>
#include <algorithm>

// Enum for selecting the filter mode
enum class FilterMode
{
    LPF12,
    HPF12,
    BPF12
};

// A zero-delay feedback (ZDF) multimode filter using the TPT (Topology-Preserving Transform) structure.
// This filter provides 12 dB/oct lowpass, highpass, and bandpass outputs, switchable via setMode().
class ZDFMultimodeFilter
{
public:
    // Constructor: sets sample rate and initializes internal state and coefficients
    ZDFMultimodeFilter();

    // Update the sample rate and recalculate coefficients
    void setSampleRate(double sr);

    // Set the cutoff frequency in Hz (limited to Nyquist range)
    void setCutoff(double freq);

    // Set the resonance amount (typically 0.0 to ~4.0 for self-oscillation)
    void setResonance(double res)
    {
        resonance = std::clamp(res, 0.0, 4.0);
        computeCoefficients();
    }

    // Choose filter mode: LPF12, BPF12, or HPF12
    void setMode(FilterMode mode);

    // Reset the internal state variables
    void reset();

    // Processes a stereo sample using mid/side filtering.
    // The ZDF filter is applied only to the mid component to preserve stereo width.
    void getSample(double &left, double &right);

private:
    // --- Parameters ---
    double sampleRate; // Sample rate
    double cutoff;     // Cutoff frequency in Hz
    double resonance;  // Resonance amount (Q control)
    FilterMode mode;   // Selected filter mode

    // --- Internal state ---
    double ic1eq = 0.0; // Internal state for first integrator (bandpass)
    double ic2eq = 0.0; // Internal state for second integrator (lowpass)

    // --- Coefficients ---
    double g = 0.0;  // Pre-warped gain factor
    double a1 = 0.0; // Coefficient for v1
    double a2 = 0.0; // Coefficient for v3

    // Process a single audio sample and return the filtered result
    double process(double input);

    // Recalculate filter coefficients based on current cutoff and resonance
    void computeCoefficients();
};
