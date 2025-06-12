#pragma once

#include <stddef.h>
#include <string>
#include "dsp_types.h"

class DSP
{
public:
    // Logger function for audio host
    using LogFunc = void (*)(const std::string &);

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

    // Log function callback registration
    static void registerLogger(LogFunc func);

protected:
    // Log function
    void log(const char* fmt, ...) const;

private:
    // Logging callback for audio host system
    static LogFunc logger;
};