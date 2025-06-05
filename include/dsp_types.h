// === In einem zentralen Header, z. B. dsp_types.h ===
#pragma once

// Umschaltbarer Floating-Point-Typ
#ifdef USE_DOUBLE_PRECISION
using dsp_float = double;
#else
using dsp_float = float;
#endif
