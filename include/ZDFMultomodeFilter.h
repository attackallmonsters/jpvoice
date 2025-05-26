#pragma once
#include <cmath>
#include <algorithm>

// Enum for selecting the filter mode
enum FilterMode { LPF12, HPF12, BPF12 };

// A zero-delay feedback (ZDF) multimode filter using the TPT (Topology-Preserving Transform) structure.
// This filter provides 12 dB/oct lowpass, highpass, and bandpass outputs, switchable via setMode().
class ZDFMultimodeFilter {
public:
    // Constructor: sets sample rate and initializes internal state and coefficients
    ZDFMultimodeFilter(double sampleRate)
        : sr(sampleRate), cutoff(1000.0), resonance(0.0), mode(LPF12)
    {
        reset();   // Clear internal filter states
        computeCoefficients(); // Initialize filter coefficients
    }

    // Update the sample rate and recalculate coefficients
    void setSampleRate(double sampleRate) {
        sr = sampleRate;
        computeCoefficients();
    }

    // Set the cutoff frequency in Hz (limited to Nyquist range)
    void setCutoff(double freq) {
        cutoff = std::clamp(freq, 10.0, sr * 0.45);
        computeCoefficients();
    }

    // Set the resonance amount (typically 0.0 to ~4.0 for self-oscillation)
    void setResonance(double res) {
        resonance = std::clamp(res, 0.0, 4.0);
        computeCoefficients();
    }

    // Choose filter mode: LPF12, BPF12, or HPF12
    void setMode(FilterMode m) {
        mode = m;
    }

    // Reset the internal state variables
    void reset() {
        ic1eq = 0.0;
        ic2eq = 0.0;
    }

    // Process a single audio sample and return the filtered result
    double process(double input) {
        // v3 is the input with negative feedback from the bandpass stage (self-resonance)
        double v3 = input - resonance * ic2eq;

        // First integrator: bandpass output
        double v1 = a1 * ic1eq + a2 * v3;

        // Second integrator: lowpass output
        double v2 = ic2eq + a2 * v1;

        // Update internal states using the trapezoidal integration formula
        ic1eq = 2.0 * v1 - ic1eq;
        ic2eq = 2.0 * v2 - ic2eq;

        // Select the desired output based on filter mode
        switch (mode) {
            case LPF12: return v2;                            // Lowpass output
            case HPF12: return v3 - resonance * v2;           // Highpass output
            case BPF12: return v1;                            // Bandpass output
            default:    return v2;                            // Fallback to lowpass
        }
    }

private:
    // --- Parameters ---
    double sr;         // Sample rate
    double cutoff;     // Cutoff frequency in Hz
    double resonance;  // Resonance amount (Q control)
    FilterMode mode;   // Selected filter mode

    // --- Internal state ---
    double ic1eq = 0.0; // Internal state for first integrator (bandpass)
    double ic2eq = 0.0; // Internal state for second integrator (lowpass)

    // --- Coefficients ---
    double g = 0.0;     // Pre-warped gain factor
    double a1 = 0.0;    // Coefficient for v1
    double a2 = 0.0;    // Coefficient for v3

    // Recalculate filter coefficients based on current cutoff and resonance
    void computeCoefficients() {
        // Bilinear transform: pre-warping for ZDF structure
        double T = 1.0 / sr;
        double wd = 2.0 * M_PI * cutoff;                  // Digital angular frequency
        double wa = (2.0 / T) * std::tan(wd * T / 2.0);   // Pre-warped analog frequency
        g = wa * T / 2.0;                                 // Filter gain

        // Coefficients for trapezoidal integrator
        a1 = 1.0 / (1.0 + g * (g + resonance));
        a2 = g * a1;
    }
};
