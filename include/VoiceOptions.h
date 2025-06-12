#pragma once

// Contains options for oscillation configuration

// OScillator types for oscillator 1
enum class CarrierOscillatiorType
{
    Saw,      // Saw oscillator
    Square,   // Sqare oscillator
    Triangle, // Triangle oscillator
    Sine      // Sine oscillator
};

// OScillator types for oscillator 1
enum class ModulatorOscillatorType
{
    Saw,      // Saw oscillator
    Square,   // Square oscillator
    Triangle, // Triangle oscsillator
    Sine      // Sine oascilator
};

// Defines the available types of noise
enum class NoiseType
{
    White, // Uncorrelated white noise
    Pink   // Filtered pink noise (equal energy per octave)
};

// FM operations modes
enum class FMType
{
    Linear,
    ThroughZero,
    Relative
};

// Enum for selecting the filter mode
enum class FilterMode
{
    LPF12,
    HPF12,
    BPF12
};