// === DSPBuffer.cpp ===
#include "DSP.h"
#include "DSPBuffer.h"
#include <algorithm>

// Constructor with optional initial size (default: 1024 samples)
DSPBuffer::DSPBuffer(size_t size = 1024)
    : buffer(size, 0.0) {}

// Resize the internal buffer and initialize new elements to 0.0
void DSPBuffer::resize(size_t newSize)
{
    buffer.resize(newSize, 0.0);
    bufferOrig = &buffer;
}

// Set all buffer elements to 0.0
void DSPBuffer::clear()
{
    std::fill(buffer.begin(), buffer.end(), 0.0);
}

// Return a mutable pointer to the internal buffer data
double *DSPBuffer::data()
{
    return buffer.data();
}

// Return a const pointer to the internal buffer data
const double *DSPBuffer::data() const
{
    return buffer.data();
}

// Return the number of elements in the buffer
size_t DSPBuffer::size() const
{
    return buffer.size();
}

// Element access by index (read/write)
double &DSPBuffer::operator[](size_t index)
{
    return buffer[index];
}

// Element access by index (read-only for const instances)
const double &DSPBuffer::operator[](size_t index) const
{
    return buffer[index];
}

// Multiply all buffer samples by a scalar gain value
void DSPBuffer::applyGain(double gain)
{
    for (auto &sample : buffer)
        sample *= gain;
}

// Copy contents from another DSPBuffer instance
void DSPBuffer::set(const DSPBuffer &other)
{
    std::copy(other.buffer.begin(), other.buffer.begin() + DSP::blockSize, buffer.begin());
}

// Copy raw data from an external float array into the buffer
void DSPBuffer::set(const float *source)
{
    for (size_t i = 0; i < DSP::blockSize; ++i)
        buffer[i] = static_cast<double>(source[i]);
}

// Copy raw data from an external double array into the buffer
void DSPBuffer::set(const double *source)
{
    std::copy(source, source + DSP::blockSize, buffer.begin());
}

// Switches the current buffer to a source buffer (shallow reference switch)
void DSPBuffer::switchTo(DSPBuffer& buf)
{
    if (!bufferOrig)
        bufferOrig = &buffer; // Save current buffer

    buffer = std::move(buf.buffer); // Move external buffer in
}

// Restores the buffer reference previously changed with switchTo
void DSPBuffer::restore()
{
    if (bufferOrig)
    {
        buffer = std::move(*bufferOrig); // Restore saved buffer
        bufferOrig = nullptr;
    }
}

// Create and return a deep copy of this buffer
DSPBuffer DSPBuffer::clone() const
{
    DSPBuffer clonedCopy;
    std::copy(buffer.begin(), buffer.begin() + DSP::blockSize, clonedCopy.buffer.begin());
    return clonedCopy;
}
