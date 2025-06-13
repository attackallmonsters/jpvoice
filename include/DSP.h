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

    // Initializes the DSP with samplerate and blocksize
    static void InitializeAudio(dsp_float rate, size_t size);

    // The max block size
    static constexpr size_t maxBlockSize = 2048;

    // The max sample rate
    static constexpr size_t maxSamplerate = 96000.0;

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