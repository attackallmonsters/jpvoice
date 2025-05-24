#pragma once

// Contains options for oscillation configuration

// OScillator types for oscillator 1
enum class CarrierOscillatiorType
{
    Supersaw    // Supersaw oascilator
};

// OScillator types for oscillator 1
enum class ModulatorOscillatorType
{
    Sine    // Sine oascilator
};

// Defines the available types of noise
enum class NoiseType
{
    White, // Uncorrelated white noise
    Pink   // Filtered pink noise (equal energy per octave)
};