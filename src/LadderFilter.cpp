#include "LadderFilter.h"
#include "clamp.h"
#include <cmath>

// Konstruktor mit Defaultwerten
LadderFilter::LadderFilter()
{
    s1L = s2L = s3L = s4L = 0.0;
    s1R = s2R = s3R = s4R = 0.0;
    cutoff = 15000.0;
    resonance = 0.0;
    drive = 1.0;

    // to avoid vtable lookup
    sampleFunc = &LadderFilter::getSampleIntern;
}

// Sets the sample rate
void LadderFilter::setSampleRate(double sr)
{
    sampleRate = clampmin(sr, 1.0);
}

// Set cutoff frequency in Hz
void LadderFilter::setCutoff(double freq)
{
    cutoff = clamp(freq, 10.0, sampleRate * 0.45);
}

// Set resonance
void LadderFilter::setResonance(double res)
{
    resonance = clamp(res, 0.0, 4.0);
}

// Set resonance
void LadderFilter::setDrive(double drv)
{
    drive = clamp(drv, 1.0, 10.0);
}

// Reset filter status
void LadderFilter::reset()
{
    s1L = s2L = s3L = s4L = 0.0;
    s1R = s2R = s3R = s4R = 0.0;
}

void LadderFilter::getSampleIntern(DSPBase *dsp, double &left, double &right)
{
    // Cast the generic DSP pointer to our specific LadderFilter type
    LadderFilter *flt = static_cast<LadderFilter *>(dsp);

    // Load the current filter states (4-stage cascade) for both left and right channels
    double s1L = flt->s1L, s2L = flt->s2L, s3L = flt->s3L, s4L = flt->s4L;
    double s1R = flt->s1R, s2R = flt->s2R, s3R = flt->s3R, s4R = flt->s4R;

    // Compute g from the cutoff frequency (normalized angular frequency)
    // g increases with higher cutoff frequency
    double g = tan(M_PI * flt->cutoff / flt->sampleRate);
    // Gain copmpensation
    double copmpensation = std::pow(0.5 + g, 2.5);

    // Compute the smoothing coefficient (1-pole lowpass response)
    double alpha = g / (1.0 + g);

    // Calculate the feedback input for the left channel:
    // subtract resonance * tanh of the last stage output (s4L)
    double inputL = static_cast<double>(left) - flt->resonance * std::tanh(flt->drive * s4L);

    // Same for the right channel
    double inputR = static_cast<double>(right) - flt->resonance * std::tanh(flt->drive * s4R);

    // Cascade 4 one-pole filters for the left channel
    s1L += alpha * (inputL - s1L);  // stage 1 reacts to input
    s2L += alpha * (s1L - s2L);     // stage 2 reacts to stage 1
    s3L += alpha * (s2L - s3L);     // and so on
    s4L += alpha * (s3L - s4L);     // final stage is the filter output

    // Same 4-stage ladder for the right channel
    s1R += alpha * (inputR - s1R);
    s2R += alpha * (s1R - s2R);
    s3R += alpha * (s2R - s3R);
    s4R += alpha * (s3R - s4R);

    // Store updated filter states back into the object
    flt->s1L = s1L; flt->s2L = s2L; flt->s3L = s3L; flt->s4L = s4L;
    flt->s1R = s1R; flt->s2R = s2R; flt->s3R = s3R; flt->s4R = s4R;

    // Return the final stage as the filter output
    left = s4L * copmpensation;
    right = s4R * copmpensation;
}
