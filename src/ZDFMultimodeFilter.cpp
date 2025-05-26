#include "ZDFMultomodeFilter.h"
#include "clamp.h"

ZDFMultimodeFilter::ZDFMultimodeFilter()
{
    sampleRate = 41100;
    cutoff = 10000.0;
    resonance = 0.0;
    mode = FilterMode::LPF12;
    reset();               // Clear internal filter states
    computeCoefficients(); // Initialize filter coefficients
}

// Choose filter mode: LPF12, BPF12, or HPF12
void ZDFMultimodeFilter::setMode(FilterMode mode)
{
    mode = mode;
}

// Reset the internal state variables
void ZDFMultimodeFilter::reset()
{
    ic1eq = 0.0;
    ic2eq = 0.0;
}

// Set the cutoff frequency in Hz (limited to Nyquist range)
void ZDFMultimodeFilter::setCutoff(double freq)
{
    cutoff = std::clamp(freq, 10.0, sampleRate * 0.45);
    computeCoefficients();
}

// Update the sample rate and recalculate coefficients
void ZDFMultimodeFilter::setSampleRate(double sr)
{
    sampleRate = sr;
    computeCoefficients();
}

// Processes a stereo sample using mid/side filtering.
// The ZDF filter is applied only to the mid component to preserve stereo width.
void ZDFMultimodeFilter::getSample(double &left, double &right)
{
    // Convert stereo input to mid/side components:
    // 'mid' represents the mono center (shared between left and right),
    // 'side' represents the stereo difference (spread between channels).
    double mid = 0.5 * (left + right);
    double side = 0.5 * (left - right);

    // Apply the zero-delay feedback filter only to the mid component.
    // This simulates a single shared filter, as in classic analog synths,
    // while preserving the stereo spread in the side signal.
    mid = process(mid);

    // Reconstruct the stereo signal by combining the filtered mid and unfiltered side.
    // This maintains the stereo width and places the filter effect in the phantom center.
    left = mid + side;
    right = mid - side;
}

// Process a single audio sample and return the filtered result
double ZDFMultimodeFilter::process(double input)
{
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
    switch (mode)
    {
    case FilterMode::LPF12:
        return v2; // Lowpass output
    case FilterMode::HPF12:
        return v3 - resonance * v2; // Highpass output
    case FilterMode::BPF12:
        return v1; // Bandpass output
    default:
        return v2; // Fallback to lowpass
    }
}

// Recalculate filter coefficients based on current cutoff and resonance
void ZDFMultimodeFilter::computeCoefficients()
{
    // Bilinear transform: pre-warping for ZDF structure
    double T = 1.0 / sampleRate;
    double wd = 2.0 * M_PI * cutoff;                // Digital angular frequency
    double wa = (2.0 / T) * std::tan(wd * T / 2.0); // Pre-warped analog frequency
    g = wa * T / 2.0;                               // Filter gain

    // Coefficients for trapezoidal integrator
    a1 = 1.0 / (1.0 + g * (g + resonance));
    a2 = g * a1;
}