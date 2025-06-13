#include <cstring>
#include <cstdarg>
#include <mutex>
#include "clamp.h"
#include "DSP.h"
#include "dsp_types.h"

size_t DSP::blockSize = 64;
dsp_float DSP::sampleRate = -1.0;
bool DSP::isInitialized = false;

// Dummy logger, does nothing
static void defaultLogger(const std::string &)
{
}

DSP::LogFunc DSP::logger = &defaultLogger;

// Contructor
DSP::DSP()
{
}

// Destructor
DSP::~DSP()
{
}

// Initializes a DSP-Object
void DSP::Initialize()
{
}

// Initializes the DSP with samplerate and blocksize
void DSP::InitializeAudio(dsp_float rate, size_t size)
{
    if (isInitialized)
        return;

    sampleRate = clamp(rate, 1.0, maxSamplerate);
    blockSize = clamp(size, static_cast<size_t>(1), maxBlockSize);

    DSP::log("DSP audio settings: samplerate is %f", sampleRate);
    DSP::log("DSP audio settings: block size is %i", blockSize);

    isInitialized = true;
}

// Log function callback registration
void DSP::registerLogger(LogFunc func)
{
    logger = func;
}

void DSP::log(const char *fmt, ...)
{
    char buffer[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    logger(std::string(buffer));
}

// Logging to file "dsp.log"
void DSP::log2File(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(logFileMutex);

    const char *logFileName = "dsp.log";

    // Only delete on first call
    FILE *file = std::fopen(logFileName, logFileInitialized ? "a" : "w");
    if (!file)
        return;

    logFileInitialized = true;

    va_list args;
    va_start(args, fmt);
    std::vfprintf(file, fmt, args);
    std::fprintf(file, "\n");
    va_end(args);

    std::fclose(file);
}

// Zeros a value if it is in the range of +/- epsilon
dsp_float DSP::zeroSubnormals(dsp_float value)
{
    return (std::abs(value) < epsilon) ? 0.0 : value;
}