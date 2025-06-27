#pragma once

#include "DSPObject.h"
#include "DSPBuffer.h"
#include "clamp.h"
#include <cmath>
#include <cstdlib>
#include <functional>

enum class LFOType {
    Sine,
    RampUp,
    RampDown,
    Triangle,
    Square,
    Random
};

class LFO : public DSPObject
{
public:
    LFO();

    virtual void initialize() override;

    void setFreq(double f);
    void setType(LFOType type);
    void setOffset(double f);
    void setDepth(double f);
    void setShape(double f);
    void setPulseWidth(double f);
    void setSmooth(double f);
    void setIdleSignal(double f);
    void reset();

    void setModBuffer(float* buffer);
    dsp_float* getBuffer();

    std::function<void()> onPhaseWrap;

private:
    static void processBlock(DSPObject* dsp);

    // Current sample rate derived values
    double phase;
    double freq;
    double phaseInc;
    double samplerate;
    double offset;
    double depth;
    double shape;
    double pw;
    double smoothVal;
    double smoothCoeff;
    double idleSignal;

    DSPBuffer lfoBuffer;
    DSPBuffer modBuffer;

    LFOType lfoType;

    // Internal waveform calculation functions
    double lfoSine();
    double lfoRampUp();
    double lfoRampDown();
    double lfoTriangle();
    double lfoSquare();
    double lfoRandom();
    double shapedRamp(double x);

    double (LFO::*lfoFunc)() = nullptr;
};