#pragma once

#include <stddef.h>
#include <string>
#include <mutex>
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

    // Log function callback registration
    static void registerLogger(LogFunc func);

    // Log function
    static void log(const char *fmt, ...);

    // Log to file function
    static void log2File(const char *fmt, ...);

    // Zeros a value if it is in the range +/- epsilon
    static dsp_float zeroSubnormals(dsp_float value);

    static bool dspIsInitialized() { return isInitialized; };

    // The max block size
    static constexpr size_t maxBlockSize = 2048;

    // The max sample rate
    static constexpr dsp_float maxSamplerate = 96000.0;

    // Threshold for zeroing
    static constexpr dsp_float epsilon = 1e-10;

    // The audio systems current sampling rate
    static dsp_float sampleRate;

    // The audio systems current sample block size
    static size_t blockSize;

private:
    // Indicator if DSP has been initialized
    static bool isInitialized;

    // Logging callback for audio host system
    static LogFunc logger;
    static inline bool logFileInitialized = false;
    static inline std::mutex logFileMutex;
};