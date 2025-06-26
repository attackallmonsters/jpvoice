#pragma once

#include "clamp.h"
#include "DSPObject.h"
#include "DSPBuffer.h"
#include <cmath>

// Enumeration for the envelope phase
enum class ADSRPhase
{
    Idle,
    Startup,
    Attack,
    Decay,
    Sustain,
    Release
};

class ADSR : public DSPObject
{
public:
    ADSR();
    
    // Initializes the ADSR
    virtual void Initialize() override;

    void setAttack(double ms);
    void setDecay(double ms);
    void setSustain(double level);
    void setRelease(double ms);
    void setAttackShape(double shape);
    void setReleaseShape(double shape);
    void setGain(double g);
    void setOneShot(bool b);
    void setStartAtCurrent(bool start);

    dsp_float* getBuffer();

    void triggerStart();
    void triggerStop();

private:
    // Next sample block generation
    static void processBlock(DSPObject *dsp);

    // Phase function (startup, attack, decay, sustain, release)
    using PhaseFunc = void (ADSR::*)();
    PhaseFunc phaseFunc;

    // Envelope values
    double attackTime, decayTime, sustainLevel, releaseTime;
    double attackShape, releaseShape;
    double currentEnv, phaseStartEnv;
    double gain;
    bool oneShot;
    bool startAtCurrentEnv;

    int attackSamples, decaySamples, releaseSamples, startupSamples;

    static constexpr int startupTimeMS = 3;

    void enterPhase(ADSRPhase newPhase);

    void phaseIdle();
    void phaseStartup();
    void phaseAttack();
    void phaseDecay();
    void phaseSustain();
    void phaseRelease();

    static double powerLerp(double start, double end, double p, double shape);
    static double shapeToExponent(double f);

    double sampleRateMS;
    ADSRPhase phase;
    int currentSample;
    DSPBuffer curveBuffer;
};
