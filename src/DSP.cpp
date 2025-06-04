#include "clamp.h"
#include "DSP.h"
#include <cstring>

size_t DSP::blockSize = 64;
double DSP::sampleRate = 44100.0;

// Contructor
DSP::DSP()
{
    sampleRate = 44100.0;
}

// Destructor
DSP::~DSP()
{
}

// Sets the block size
void DSP::setBlockSize(size_t size)
{
    blockSize = size;
}

// Sets the audio systems current sampling rate
void DSP::setSampleRate(double rate)
{
    sampleRate = clampmin(rate, 1.0);
}