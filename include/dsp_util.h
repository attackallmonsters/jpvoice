#pragma once

inline double fast_tanh(double val)
{
    if (val < -3.0) return -1.0;
    if (val > 3.0)  return 1.0;

    const double val2 = val * val;
    return val * (27.0 + val2) / (27.0 + 9.0 * val2);
}