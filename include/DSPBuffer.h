// DSPBuffer: A utility class for managing a dynamic buffer of double-precision audio samples.
// Provides common operations such as resize, clear, gain application, and deep copying.

#pragma once
#include <vector>
#include <cstddef>

class DSPBuffer
{
public:
    // Constructor with optional initial size (default: 1024 samples)
    explicit DSPBuffer(size_t size);

    // Resize the internal buffer and initialize new elements to 0.0
    void resize(size_t newSize);

    // Set all buffer elements to 0.0
    void clear();

    // Return a mutable pointer to the internal buffer data
    double *data();

    // Return a const pointer to the internal buffer data
    const double *data() const;

    // Return the number of elements in the buffer
    size_t size() const;

    // Element access by index (read/write)
    double &operator[](size_t index);

    // Element access by index (read-only for const instances)
    const double &operator[](size_t index) const;

    // Multiply all buffer samples by a scalar gain value
    void applyGain(double gain);

    // Copy contents from another DSPBuffer instance
    void set(const DSPBuffer &other);

    // Copy raw data from an external float array into the buffer
    void set(const float *source);

    // Copy raw data from an external double array into the buffer
    void set(const double *source);

    // Switches the current buffer to a source buffer.
    void switchTo(DSPBuffer &buf);

    // Restores the buffer reference previously changed with switchTo.
    void restore();

    // Create and return a deep copy of this buffer
    DSPBuffer clone() const;

private:
    std::vector<double> buffer;               // Internal buffer storage
    std::vector<double> *bufferOrig = nullptr; // Internal buffer storage
};
