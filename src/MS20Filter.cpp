// === ms20filter.cpp ===
#include "MS20Filter.h"
#include "DSPObject.h"
#include "dsp_util.h"
#include <cmath>

// Constructor with sample rate
MS20Filter::MS20Filter()
{
    y1L = y1R = y2L = y2R = 0.0;
    T = 1.0 / DSP::sampleRate;
    registerBlockProcessor(&MS20Filter::processBlock);
}

// Set the cutoff frequency and update coefficients
void MS20Filter::setCutoff(DSPBuffer *buffer)
{
    cutoffBuffer = buffer;
}

// Set the resonance amount
void MS20Filter::setResonance(DSPBuffer *buffer)
{
    resoBuffer = buffer;
}

// Assigns the samples to process
void MS20Filter::setSampleBuffers(DSPBuffer *samplesL, DSPBuffer *samplesR)
{
    bufferL = samplesL;
    bufferR = samplesR;
}

// Process a single sample through the MS-20 style lowpass filter
void MS20Filter::processBlock(DSPObject *dsp)
{
    MS20Filter *flt = static_cast<MS20Filter *>(dsp);

    size_t blocksize = DSP::blockSize;
    double left, right;
    double cutoff, reso;
    double wc, alpha;
    double feedback;
    double x1;
    double y1L = flt->y1L;
    double y2L = flt->y2L;
    double y1R = flt->y1R;
    double y2R = flt->y2R;
    double T = flt->T;

    for (size_t i = 0; i < blocksize; ++i)
    {
        left = (*flt->bufferL)[i];
        right = (*flt->bufferR)[i];
        cutoff = (*flt->cutoffBuffer)[i];
        reso = (*flt->resoBuffer)[i];

        // Calculate coefficient based on cutoff
        wc = 2.0 * M_PI * cutoff;

        alpha = wc * T / (1.0 + wc * T); // Bilinear transform approximation

        // === left ===
        feedback = fast_tanh(reso * nonlinearFeedback(y2L - left));

        // First integrator (emulating Sallen-Key stage)
        x1 = left - feedback;
        y1L += alpha * (x1 - y1L);

        // Second integrator
        y2L += alpha * (y1L - y2L);

        (*flt->bufferL)[i] = y2L;

        // === right ===
        feedback = fast_tanh(reso * nonlinearFeedback(y2R - right));

        // First integrator (emulating Sallen-Key stage)
        x1 = right - feedback;
        y1R += alpha * (x1 - y1R);

        // Second integrator
        y2R += alpha * (y1R - y2R);

        (*flt->bufferR)[i] = y2R;
    }

    flt->y1L = y1L;
    flt->y2L = y2L;
    flt->y1R = y1R;
    flt->y2R = y2R;
}

// Optional: reset internal state variables
void MS20Filter::reset()
{
    y1L = 0.0;
    y2L = 0.0;
    y1R = 0.0;
    y2R = 0.0;
}

// Emulate diode clipping behavior with tanh nonlinearity
double MS20Filter::nonlinearFeedback(double s)
{
    return s * 1.5; // Gain scales the nonlinearity
}
