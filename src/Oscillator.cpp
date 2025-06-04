#include "Oscillator.h"
#include "clamp.h"

// no FM
static double fmNone(const double &base, const double &/*mod*/, const double &/*index*/)
{
    return base;
}

// Linear FM
static double fmLinear(const double &base, const double &mod, const double &index)
{
    return base + mod * index;
}

// Relative FM
static double fmRelative(const double &base, const double &mod, const double &index)
{
    return base + mod * index * base;
}

// Trough Zero FM
static double fmThroughZero(const double &base, const double &mod, const double &index)
{
    return base + mod * index;
}

Oscillator::Oscillator()
{
    // to avoid vtable lookup
    sampleFunc = &Oscillator::setSamplesIntern;
    computeSampleFunc = &Oscillator::noopComputeSampleFunc;
}

// Resets the internal oscillator phase to 0.0.
void Oscillator::resetPhase()
{
    currentPhase = 0.0;
    wrapped = false;
}

// Sets the desired oscillator frequency in Hertz
void Oscillator::setFrequency(double value)
{
    frequency = clampmin(value, 0.0);
    setCalculatedFrequency(frequency);
}

// Sets the pitch offset in semi tones
void Oscillator::setPitchOffset(int value)
{
    pitchOffset = clamp(value, -24, 24);
    setCalculatedFrequency(frequency);
}

// Sets the fine tuning in cent
void Oscillator::setFineTune(double value)
{
    fineTune = clamp(value, -100.0, 100.0);
    setCalculatedFrequency(frequency);
}

// Enables negative phase wrapping
void Oscillator::setNegativeWrappingEnabled(bool enabled)
{
    negativeWrappingEnabled = enabled;
}

// Calculates the effective frequency based on base frequency,
// pitch offset (in semitones), and fine-tuning (in cents).
// Then updates the phase increment accordingly.
void Oscillator::setCalculatedFrequency(double f)
{
    // Convert pitch offset and fine tune to a total semitone offset
    double semitoneOffset = static_cast<double>(pitchOffset) + (fineTune / 100.0);

    // Apply equal temperament formula: f = f0 * 2^(n/12)
    calculatedFrequency = f * std::pow(2.0, semitoneOffset / 12.0);

    // Update phase increment for waveform generation
    phaseIncrement = calculatedFrequency / DSP::sampleRate;
}

// Gets the current frequency
double Oscillator::getFrequency()
{
    return frequency;
}

// Gets the calculated FM frequency
double Oscillator::getCalculatedFrequency()
{
    return calculatedFrequency;
}

// Returns true if the oscillator's phase wrapped during the last getSample() call
bool Oscillator::hasWrapped()
{
    return wrapped;
}

// Resets the wrap status
void Oscillator::unWrap()
{
    wrapped = false;
}

// Sets the type of FM to use
void Oscillator::setFMType(FMType fm)
{
    fmType = fm;
    negativeWrappingEnabled = fmType == FMType::ThroughZero;
    setNegativeWrappingEnabled(negativeWrappingEnabled);

    if (fmType == FMType::Relative && modulationIndex > 30.0)
    {
        setFMModIndex(30);
    }

    switch (fmType)
    {
    case FMType::Linear:
        fmFunc = fmLinear;
        break;
    case FMType::Relative:
        fmFunc = fmRelative;
        break;
    case FMType::ThroughZero:
        fmFunc = fmThroughZero;
        break;
    default:
        fmFunc = fmNone;
        break;
    }
}

// Sets the modulation index for frequency modulation.
// This controls the intensity of the frequency modulation effect.
void Oscillator::setFMModIndex(double index)
{
    double modmax = (fmType == FMType::Relative) ? 30 : 1000;
    modulationIndex = clamp(index, 0.0, modmax);

    if (modulationIndex == 0)
    {
        setFrequency(frequency);
        return;
    }
}

// Dummy ComputeSampleFunc for setSamples
void Oscillator::noopComputeSampleFunc(Oscillator *, const double &phase, double &left, double &right)
{
    left = right = 2.0 * phase - 1.0; // Generates a sawtooth
}

// Next sample block generation
void Oscillator::setSamplesIntern(DSPObject *)
{

}

