#ifndef DCLANG_FAST_MATH_H
#define DCLANG_FAST_MATH_H

#include <math.h>

#define DCLANG_FAST_PI 0x1.921fb54442d18p+1
#define DCLANG_FAST_TWO_PI 0x1.921fb54442d18p+2
#define DCLANG_FAST_INV_TWO_PI 0x1.45f306dc9c883p-3

/* Ninth-order sine after folding a normalized phase into [-pi/2, pi/2].
 * The DSP kernels already keep phase in [-1, 1), so their hot path avoids
 * general range reduction. Maximum absolute error is about 3.55e-6. */
static inline double dclang_fast_sin_unit(double phase)
{
    if (phase < -0.5)
        phase += 1.0;
    else if (phase > 0.5)
        phase -= 1.0;
    double x = phase * DCLANG_FAST_TWO_PI;
    if (x < -0.5 * DCLANG_FAST_PI)
        x = -DCLANG_FAST_PI - x;
    else if (x > 0.5 * DCLANG_FAST_PI)
        x = DCLANG_FAST_PI - x;
    double square = x * x;
    return x * (1.0 + square * (-1.0 / 6.0 + square * (1.0 / 120.0
           + square * (-1.0 / 5040.0 + square * (1.0 / 362880.0)))));
}

static inline double dclang_fast_sin(double radians)
{
    double phase = radians * DCLANG_FAST_INV_TWO_PI;
    if (!(phase > -1.0 && phase < 1.0))
        phase = fmod(phase, 1.0);
    return dclang_fast_sin_unit(phase);
}

/* [7/6] Pade approximation with explicit saturation. Maximum absolute
 * error on [-4, 4] is about 6.71e-4 and is much smaller near the origin. */
static inline double dclang_fast_tanh(double x)
{
    if (x <= -4.0)
        return -1.0;
    if (x >= 4.0)
        return 1.0;
    double square = x * x;
    double numerator = x * (135135.0
        + square * (17325.0 + square * (378.0 + square)));
    double denominator = 135135.0
        + square * (62370.0 + square * (3150.0 + 28.0 * square));
    return numerator / denominator;
}

#endif
