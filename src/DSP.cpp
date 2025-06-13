#include <cstring>
#include <cstdarg>
#include "clamp.h"
#include "DSP.h"
#include "dsp_types.h"

size_t DSP::blockSize = 64;
dsp_float DSP::sampleRate = 44100.0;

// Dummy logger, does nothing
static void defaultLogger(const std::string &)
{
}

DSP::LogFunc DSP::logger = &defaultLogger;

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

// Initializes the DSP with samplerate and blocksize
void DSP::InitializeAudio(dsp_float rate, size_t size)
{
    sampleRate = clamp(rate, 1.0, maxSamplerate);
    blockSize = clamp(size, static_cast<size_t>(1), maxBlockSize);
}

// Log function callback registration
void DSP::registerLogger(LogFunc func)
{
    logger = func;
}

void DSP::log(const char *fmt, ...) const
{
    char buffer[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    logger(std::string(buffer));
}