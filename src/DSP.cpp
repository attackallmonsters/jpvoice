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