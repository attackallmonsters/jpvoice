#pragma once

#include <stddef.h>
#include "dsp_types.h"

class DSP
{
public:
    // Ctor
    DSP();

    // Dtor
    virtual ~DSP();

    // Initializes a DSP-Object
    virtual void Initialize();

    // Sets the block size
    static void setBlockSize(size_t size);

    // Sets the sample rate
    static void setSampleRate(dsp_float rate);

    // The max block size
    static constexpr size_t maxBlockSize = 2048;

    // The audio systems current sampling rate
    static dsp_float sampleRate; 

    // The audio systems current sample block size
    static size_t blockSize;  
};