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
    filterStage = FilterStage::TwoPole;

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
    resonance = clamp(res, 0.0, 6.0);
}

// Set resonance
void LadderFilter::setDrive(double drv)
{
    drive = clamp(drv, 1.0, 20.0);
}

// Reset filter status
void LadderFilter::reset()
{
    s1L = s2L = s3L = s4L = 0.0;
    s1R = s2R = s3R = s4R = 0.0;
}

// Sets the filter stage
void LadderFilter::setFilterStage(FilterStage stage)
{
    filterStage = stage;
}

void LadderFilter::getSampleIntern(DSPBase *dsp, double &left, double &right)
{
    LadderFilter *flt = static_cast<LadderFilter *>(dsp);

    // Load previous filter states
    double s1L = flt->s1L, s2L = flt->s2L, s3L = flt->s3L, s4L = flt->s4L;
    double s1R = flt->s1R, s2R = flt->s2R, s3R = flt->s3R, s4R = flt->s4R;

    double g = tan(M_PI * flt->cutoff / flt->sampleRate);
    double alpha = g / (1.0 + g);

    // Normalize cutoff for output compensation
    double norm = std::clamp(flt->cutoff / (flt->sampleRate * 0.5), 0.01, 1.0);
    double comp = std::pow(1.0 - norm, 0.4);
    comp = comp * 0.7 + 0.3;

    // === Left channel ===
    double inL = static_cast<double>(left);

    // Feedback depends on mode
    double feedbackL = 0.0;
    if (flt->filterStage == FilterStage::FourPole)
        feedbackL = flt->resonance * std::tanh(flt->drive * s4L);
    else
        feedbackL = flt->resonance * 1.8 * std::tanh(flt->drive * s2L); // boost for 2-pole

    double inputL = std::tanh(flt->drive * inL) - feedbackL;

    s1L += alpha * (inputL - s1L);
    s2L += alpha * (s1L - s2L);
    s3L += alpha * (s2L - s3L);
    s4L += alpha * (s3L - s4L);

    // === Right channel ===
    double inR = static_cast<double>(right);
    double feedbackR = 0.0;

    if (flt->filterStage == FilterStage::FourPole)
        feedbackR = flt->resonance * std::tanh(flt->drive * s4R);
    else
        feedbackR = flt->resonance * 1.8 * std::tanh(flt->drive * s2R);

    double inputR = std::tanh(flt->drive * inR) - feedbackR;

    s1R += alpha * (inputR - s1R);
    s2R += alpha * (s1R - s2R);
    s3R += alpha * (s2R - s3R);
    s4R += alpha * (s3R - s4R);

    // Store updated states
    flt->s1L = s1L; flt->s2L = s2L; flt->s3L = s3L; flt->s4L = s4L;
    flt->s1R = s1R; flt->s2R = s2R; flt->s3R = s3R; flt->s4R = s4R;

    // Select output depending on mode
    double outL = (flt->filterStage == FilterStage::FourPole) ? s4L : s2L;
    double outR = (flt->filterStage == FilterStage::FourPole) ? s4R : s2R;

    // Output soft clip with gain compensation
    left  = std::tanh(outL * comp);
    right = std::tanh(outR * comp);
}

