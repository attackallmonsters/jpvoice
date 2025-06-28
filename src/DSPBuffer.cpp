#include "DSPBuffer.h"
#include "DSP.h"
#include <algorithm>
#include <cstring>

// Constructor: empty state
DSPBuffer::DSPBuffer() = default;

// Move constructor
DSPBuffer::DSPBuffer(DSPBuffer &&other) noexcept
{
    *this = std::move(other);
}

// Destructor: free memory if owned
DSPBuffer::~DSPBuffer()
{
    release();
}

// Create owned buffer, zero-initialized
void DSPBuffer::create(size_t size, dsp_float f)
{
    release();

    buffer = new dsp_float[size];
    
    bufferSize = size;
    ownsBuffer = true;

    fill(f);
}

// Use external buffer (no ownership)
void DSPBuffer::set(DSPBuffer &externalBuffer)
{
    release();

    buffer = externalBuffer.data();
    bufferSize = externalBuffer.size();
    ownsBuffer = false;
}

// Use external buffer (no ownership)
void DSPBuffer::set(dsp_float *externalBuffer, size_t size)
{
    release();

    buffer = externalBuffer;
    bufferSize = size;
    ownsBuffer = false;
}

// Fills the buffer with a value
void DSPBuffer::fill(dsp_float f)
{
    std::fill(buffer, buffer + bufferSize, f);
}

// Frees buffer if owned
void DSPBuffer::release()
{
    if (ownsBuffer && buffer)
    {
        delete[] buffer;
    }

    buffer = nullptr;
    bufferSize = 0;
    ownsBuffer = false;
}

// Direct access
dsp_float *DSPBuffer::data()
{
    return buffer;
}

const dsp_float *DSPBuffer::data() const
{
    return buffer;
}

size_t DSPBuffer::size() const
{
    return bufferSize;
}

// Array access
dsp_float &DSPBuffer::operator[](size_t index)
{
    return buffer[index];
}

const dsp_float &DSPBuffer::operator[](size_t index) const
{
    return buffer[index];
}

// Set all values to 0
void DSPBuffer::clear()
{
    if (buffer)
        std::fill(buffer, buffer + bufferSize, 0.0f);
}

// Copy values from external buffer
void DSPBuffer::copyFrom(const dsp_float *source)
{
    if (buffer && source)
        std::copy(source, source + bufferSize, buffer);
}

// Query ownership
bool DSPBuffer::owns() const
{
    return ownsBuffer;
}

DSPBuffer &DSPBuffer::operator=(dsp_float *ptr)
{
    set(ptr, DSP::blockSize);
    return *this;
}

// Move assignment
DSPBuffer &DSPBuffer::operator=(DSPBuffer &&other) noexcept
{
    if (this != &other)
    {
        release();
        buffer = other.buffer;
        bufferSize = other.bufferSize;
        ownsBuffer = other.ownsBuffer;

        other.buffer = nullptr;
        other.bufferSize = 0;
        other.ownsBuffer = false;
    }
    return *this;
}
