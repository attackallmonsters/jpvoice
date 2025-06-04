#pragma once

#include <stddef.h>

class DSP
{
public:
    // Ctor
    DSP();

    // Dtor
    virtual ~DSP();

    // Sets the block size
    static void setBlockSize(size_t size);

    // Sets the sample rate
    static void setSampleRate(double rate);

    // The max block size
    static constexpr int maxBlockSize = 1024; // Buffer size

    static double sampleRate; // The audio systems current sampling rate
    static size_t blockSize;  // The sample buffer size
};