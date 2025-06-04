#include "clamp.h"
#include "DSPObject.h"
#include <cstring>

// Contructor
DSPObject::DSPObject()
{
    sampleFunc = noopSampleFunc;
}

// Destructor
DSPObject::~DSPObject()
{
}

// Generates the next audio sample block
void DSPObject::setSamples()
{
    (*sampleFunc)(this);
}

// Dummy SampleFunc
void DSPObject::noopSampleFunc(DSPObject *)
{
}