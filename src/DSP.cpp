#include "clamp.h"
#include "DSP.h"
#include <cstring>
#include "dsp_types.h"

size_t DSP::blockSize = 64;
dsp_float DSP::sampleRate = 44100.0;

// Contructor
DSP::DSP()
{
    sampleRate = 44100.0;
}

// Destructor
DSP::~DSP()
{
}

// Initializes a DSP-Object
void DSP::Initialize()
{
}

// Sets the block size
void DSP::setBlockSize(size_t size)
{
    blockSize = size;
}

// Sets the audio systems current sampling rate
void DSP::setSampleRate(dsp_float rate)
{
    sampleRate = clampmin(rate, 1.0);
}