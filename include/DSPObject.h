#pragma once

#include <stddef.h>
#include "DSP.h"
#include "DSPBuffer.h"

class DSPObject : public DSP
{
public:
    // Ctor
    DSPObject();

    // Dtor
    virtual ~DSPObject();

    // Calculates the next sample buffer
    void setSamples();
    
protected:
    // Avoid vtable lookup
    using SampleFunc = void (*)(DSPObject *);
    SampleFunc sampleFunc;

private:
    // Dummy SampleFunc for setSamples
    static void noopSampleFunc(DSPObject *);
};