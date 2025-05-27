#include "ZDFMultomodeFilter.h"
#include "clamp.h"
#include <cmath>

// Konstruktor mit Defaultwerten
ZDFMultimodeFilter::ZDFMultimodeFilter()
{
    sampleRate = 48000.0;
    cutoff = 5000.0;
    resonance = 1.0;
    filterMode = FilterMode::LPF12;
    reset();
    computeCoefficients();
}

// Setzt Filtermodus
void ZDFMultimodeFilter::setMode(FilterMode mode)
{
    filterMode = mode;
}

// Rücksetzen interner Zustände
void ZDFMultimodeFilter::reset()
{
    ic1eq = 0.0;
    ic2eq = 0.0;
}

// Setzt Grenzfrequenz in Hz
void ZDFMultimodeFilter::setCutoff(double freq)
{
    cutoff = std::clamp(freq, 10.0, sampleRate * 0.45);
    computeCoefficients();
}

// Setzt Resonanz (Q)
void ZDFMultimodeFilter::setResonance(double res)
{
    resonance = std::clamp(res, 0.0, 4.0);
    computeCoefficients();
}

// Setzt Sample Rate in Hz
void ZDFMultimodeFilter::setSampleRate(double sr)
{
    sampleRate = sr;
    computeCoefficients();
}

// Stereo Mid/Side Verarbeitung
void ZDFMultimodeFilter::getSample(double &left, double &right)
{
    double mid = 0.5 * (left + right);
    double side = 0.5 * (left - right);

    mid = process(mid);

    left = mid + side;
    right = mid - side;
}

// Verarbeitung eines einzelnen Samples (Zavalishin ZDF SVF)
double ZDFMultimodeFilter::process(double input)
{
    double vhp = (input - R * bp - lp) * g;
    bp += vhp;
    lp += bp;

    switch (filterMode)
    {
    case FilterMode::LPF12:
        return lp;
    case FilterMode::BPF12:
        return bp;
    case FilterMode::HPF12:
        return vhp;
    default:
        return lp;
    }
}

// Berechne Koeffizienten für ZDF-SVF
void ZDFMultimodeFilter::computeCoefficients()
{
    double wd = 2.0 * M_PI * cutoff;
    double T = 1.0 / sampleRate;
    double wa = (2.0 / T) * std::tan(wd * T / 2.0); // pre-warped frequency
    g = wa * T / 2.0;
    h = g / (1.0 + g); // trapezoidal integration coefficient
    R = resonance;     // feedback amount (Q inverse)
}
