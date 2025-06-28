#include "LFO.h"

LFO::LFO()
{
    registerBlockProcessor(&LFO::processBlock);
}

void LFO::initialize()
{
    samplerate = DSP::sampleRate;
    lfoBuffer.create(DSP::blockSize);

    phase = 0.0;
    phaseInc = 0.0;
    
    setFreq(0.0);
    setOffset(0.0);
    setDepth(1.0);
    setShape(0.0);
    setPulseWidth(0.5);
    setSmooth(0.0);
    setType(LFOType::Sine);
    setFreq(1.0);
}

inline double LFO::shapedRamp(double x)
{
    if (shape == 0.0)
        return x;
    else if (shape > 0.0)
        return std::pow(x, 1.0 + shape * 4.0); // convex
    else
        return 1.0 - std::pow(1.0 - x, 1.0 - shape * 4.0); // concave
}

void LFO::setSmooth(double f)
{
    f = clamp(f, 0.0, 1.0);
    smoothCoeff = 1.0 - f;
}

inline double LFO::lfoSine()
{
    return std::sin(phase * 2.0 * M_PI);
}

inline double LFO::lfoRampUp()
{
    return 2.0 * shapedRamp(phase) - 1.0;
}

inline double LFO::lfoRampDown()
{
    return 1.0 - 2.0 * shapedRamp(phase);
}

inline double LFO::lfoTriangle()
{
    double p = phase * 2.0;
    if (p < 1.0)
        return 2.0 * shapedRamp(p) - 1.0;
    else
        return 1.0 - 2.0 * shapedRamp(p - 1.0);
}

inline double LFO::lfoSquare()
{
    return (phase < pw) ? 1.0 : -1.0;
}

inline double LFO::lfoRandom()
{
    return 2.0 * ((double)rand() / RAND_MAX) - 1.0;
}

void LFO::setFreq(double f)
{
    freq = clampmin(f, 0.0);
    phaseInc = freq / samplerate;
}

void LFO::setType(LFOType t)
{
    lfoType = t;

    switch (t)
    {
    case LFOType::Sine:
        lfoFunc = &LFO::lfoSine;
        break;
    case LFOType::RampUp:
        lfoFunc = &LFO::lfoRampUp;
        break;
    case LFOType::RampDown:
        lfoFunc = &LFO::lfoRampDown;
        break;
    case LFOType::Triangle:
        lfoFunc = &LFO::lfoTriangle;
        break;
    case LFOType::Square:
        lfoFunc = &LFO::lfoSquare;
        break;
    case LFOType::Random:
        lfoFunc = &LFO::lfoRandom;
        break;
    default:
        lfoFunc = &LFO::lfoSine;
        break;
    }
}

void LFO::setOffset(double f)
{
    offset = f;
}

void LFO::setDepth(double f)
{
    depth = f;
}

void LFO::setShape(double f)
{
    shape = clamp(f, -1.0, 1.0);
}

void LFO::setPulseWidth(double f)
{
    pw = clamp(f, 0.01, 0.99);
}

void LFO::setIdleSignal(double f)
{
    idleSignal = f;
}

void LFO::reset()
{
    phase = 0.0;

    if (onPhaseWrap)
        onPhaseWrap();
}

void LFO::setModBuffer(float* buffer)
{
    
}

void LFO::processBlock(DSPObject *dsp)
{
    LFO *lfo = static_cast<LFO *>(dsp);
    size_t n = DSP::blockSize;

    if (lfo->freq <= 0.0)
    {
        lfo->phase = 0.0;
        for (size_t i = 0; i < n; ++i)
            lfo->lfoBuffer[i] = lfo->idleSignal;
        return;
    }

    double phase = lfo->phase;
    double inc = lfo->phaseInc;

    for (size_t i = 0; i < n; ++i)
    {
        lfo->phase = phase;
        double val = (lfo->*lfo->lfoFunc)();

        double target = val * lfo->depth + lfo->offset;
        lfo->smoothVal += lfo->smoothCoeff * (target - lfo->smoothVal);
        lfo->lfoBuffer[i] = lfo->smoothVal;

        phase += inc;

        if (phase >= 1.0)
        {
            phase -= 1.0;

            if (lfo->onPhaseWrap)
                lfo->onPhaseWrap();
        }
    }

    lfo->phase = phase;
}

dsp_float *LFO::getBuffer()
{
    return lfoBuffer.data();
}