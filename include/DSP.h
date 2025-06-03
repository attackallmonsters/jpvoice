#pragma once

class DSP
{
public:
    // Ctor
    DSP();

    // Dtor
    virtual ~DSP();

    // Sets the block size
    static void setBlockSize(int size);

    // Sets the sample rate
    static void setSampleRate(double rate);

    // Gets the current sample rate
    double getSampleRate() const;

    // Generates the next audio sample block
    void setSamples();

    // Copies external buffers to the internal buffers
    void copyBuffer(double *srcBufL, double *srcBufR);

    static constexpr int maxBlockSize = 1024; // Buffer size
    double BufferLeft[maxBlockSize];          // Left multi perpose audio buffer
    double BufferRight[maxBlockSize];         // Right multi purpose audio buffer

protected:
    static double sampleRate; // The audio systems current sampling rate
    static int blockSize;     // Sample buffer size

    // Avoid vtable lookup
    using SampleFunc = void (*)(DSP *);
    SampleFunc sampleFunc;

private:
    // Dummy SampleFunc for setSamples
    static void noopSampleFunc(DSP *);
};