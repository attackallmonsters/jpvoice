#pragma once

#include "dsp_types.h"
#include <cstddef>

class DSPBuffer {
public:
    DSPBuffer();
    ~DSPBuffer();

    // Create internal, zero-initialized buffer with ownership
    void create(size_t size, dsp_float f = 0.0);

    // Set external buffer (no ownership)
    void set(DSPBuffer &externalBuffer);

    // Set external buffer (no ownership)
    void set(dsp_float* externalBuffer, size_t size);

    // Fills the buffer with a value
    void fill(dsp_float f);

    // Free internal buffer if owned
    void release();

    // Buffer access
    dsp_float* data();
    const dsp_float* data() const;
    size_t size() const;

    // Access via operator[]
    dsp_float& operator[](size_t index);
    const dsp_float& operator[](size_t index) const;

    // Set all values to 0.0
    void clear();

    // Copy from external buffer (must be at least `size()` samples)
    void copyFrom(const dsp_float* source);

    // Returns true if the buffer owns its memory
    bool owns() const;

    // Assignment of fixed size buffer (DSP block size)
    DSPBuffer& operator=(dsp_float* ptr);

    // Move constructor / assignment
    DSPBuffer(DSPBuffer&& other) noexcept;
    DSPBuffer& operator=(DSPBuffer&& other) noexcept;

    // Copy disabled (ambiguous ownership)
    DSPBuffer(const DSPBuffer&) = delete;
    DSPBuffer& operator=(const DSPBuffer&) = delete;

private:
    dsp_float* buffer = nullptr;
    size_t bufferSize = 0;
    bool ownsBuffer = false;
};
