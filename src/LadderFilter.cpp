#include "LadderFilter.h"
#include "dsp_util.h"
#include "clamp.h"
#include "dsp_util.h"

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
    cutoff = clamp(freq, 1.0, sampleRate * 0.45);
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

    // Compute g from the cutoff frequency (normalized angular frequency)
    double g = tan(M_PI * flt->cutoff / DSP::sampleRate);

    // Compute the smoothing coefficient (1-pole lowpass response)
    double alpha = g / (1.0 + g);

    // Gain copmpensation
    double compensation;
    if (flt->filterStage == FilterStage::TwoPole)
        compensation = 1.0 / std::pow(1.0 + g, 0.5);
    else
        compensation = 1.0 / std::pow(1.0 + g, 1.0);

    for (int i = 0; i < DSP::blockSize; ++i)
    {
        double left = flt->BufferLeft[i];
        double right = flt->BufferRight[i];

        // Feedback calculation
        double inputL, inputR;
        if (flt->filterStage == FilterStage::TwoPole)
        {
            inputL = fast_tanh(flt->drive * 0.5 * left) - flt->resonance * fast_tanh(flt->drive * s2L);
            inputR = fast_tanh(flt->drive * 0.5 * right) - flt->resonance * fast_tanh(flt->drive * s2R);
        }
        else
        {
            inputL = fast_tanh(flt->drive * 0.5 * left) - flt->resonance * fast_tanh(flt->drive * s4L);
            inputR = fast_tanh(flt->drive * 0.5 * right) - flt->resonance * fast_tanh(flt->drive * s4R);
        }

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
            flt->BufferLeft[i] = fast_tanh(s2L * compensation);
            flt->BufferRight[i] = fast_tanh(s2R * compensation);
        }
        else
        {
            flt->BufferLeft[i] = fast_tanh(s4L * compensation);
            flt->BufferRight[i] = fast_tanh(s4R * compensation);
        }
    }

    flt->s2L = s2L;
    flt->s3L = s3L;
    flt->s4L = s4L;
    flt->s1R = s1R;
    flt->s2R = s2R;
    flt->s3R = s3R;
    flt->s4R = s4R;
}
