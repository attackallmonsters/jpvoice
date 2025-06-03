#include "clamp.h"
#include "DSP.h"
#include <cstring>

int DSP::blockSize = 64;
double DSP::sampleRate = 44100.0;

// Contructor
DSP::DSP()
{
    sampleRate = 44100.0;
    sampleFunc = noopSampleFunc;
}

// Destructor
DSP::~DSP()
{
}

// Sets the block size
void DSP::setBlockSize(int size)
{
    blockSize = size;
}

// Sets the audio systems current sampling rate
void DSP::setSampleRate(double rate)
{
    sampleRate = clampmin(rate, 1.0);
}

// Gets the current sampling rate
double DSP::getSampleRate() const
{
    return sampleRate;
}

// Copies external buffers to the internal buffers
void DSP::copyBuffer(double *srcBufL, double *srcBufR)
{
    std::memcpy(BufferLeft, srcBufL, sizeof(double) * blockSize);
    std::memcpy(BufferRight, srcBufR, sizeof(double) * blockSize);
}

// Generates the next audio sample block
void DSP::setSamples()
{
    (*sampleFunc)(this);
}

// Dummy SampleFunc
void DSP::noopSampleFunc(DSP *)
{
}