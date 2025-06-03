#include "LadderFilter.h"
#include "dsp_util.h"
#include "clamp.h"

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
    sampleFunc = &LadderFilter::setSamplesIntern;
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

// Next sample block generation
void LadderFilter::setSamplesIntern(DSP *dsp)
{
    LadderFilter *flt = static_cast<LadderFilter *>(dsp);

    // Load previous filter states
    double s1L = flt->s1L, s2L = flt->s2L, s3L = flt->s3L, s4L = flt->s4L;
    double s1R = flt->s1R, s2R = flt->s2R, s3R = flt->s3R, s4R = flt->s4R;

    double g = tan(M_PI * flt->cutoff / DSP::sampleRate);
    double alpha = g / (1.0 + g);

    // Normalize cutoff for output compensation
    double norm = std::clamp(flt->cutoff / (DSP::sampleRate * 0.5), 0.01, 1.0);
    double comp = std::pow(1.0 - norm, 0.4);

    if (flt->filterStage == FilterStage::TwoPole)
        comp *= 0.8;
    else
        comp = comp * 0.7 + 0.3;

    for (int i = 0; i < DSP::blockSize; ++i)
    {
        double left = flt->BufferLeft[i];
        double right = flt->BufferRight[i];

        // Feedback depends on mode
        double feedbackL, feedbackR;
        if (flt->filterStage == FilterStage::TwoPole)
        {
            feedbackL = flt->resonance * 2.0 * fast_tanh(flt->drive * s2L);
            feedbackR = flt->resonance * 2.0 * fast_tanh(flt->drive * s2R);
        }
        else
        {
            feedbackL = flt->resonance * fast_tanh(flt->drive * s4L);
            feedbackR = flt->resonance * fast_tanh(flt->drive * s4R);
        }

        double inputL = fast_tanh(flt->drive * left) - feedbackL;
        double inputR = fast_tanh(flt->drive * right) - feedbackR;

        s1L += alpha * (inputL - s1L);
        s2L += alpha * (s1L - s2L);
        s1R += alpha * (inputR - s1R);
        s2R += alpha * (s1R - s2R);

        if (flt->filterStage == FilterStage::FourPole)
        {
            s3L += alpha * (s2L - s3L);
            s4L += alpha * (s3L - s4L);
            s3R += alpha * (s2R - s3R);
            s4R += alpha * (s3R - s4R);
        }

        if (flt->filterStage == FilterStage::TwoPole)
        {
            flt->BufferLeft[i] = fast_tanh(s2L * comp);
            flt->BufferRight[i] = fast_tanh(s2R * comp);
        }
        else
        {
            flt->BufferLeft[i] = fast_tanh(s4L * comp);
            flt->BufferRight[i] = fast_tanh(s4R * comp);
        }
    }

    // Store updated states
    flt->s1L = s1L;
    flt->s2L = s2L;
    flt->s3L = s3L;
    flt->s4L = s4L;
    flt->s1R = s1R;
    flt->s2R = s2R;
    flt->s3R = s3R;
    flt->s4R = s4R;
}
