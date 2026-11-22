#ifndef DCLANG_OSC_H
#define DCLANG_OSC_H

#include <math.h>
#include <stddef.h>

#define DCLANG_TWO_PI 0x1.921fb54442d18p+2

/* The common cases use the same remainder as fmod(x, 1), including
 * negative zero at the boundaries handled by fmod. */
static inline double dclang_unit_wrap(double x)
{
    if (x > -1.0 && x < 1.0)
        return x;
    if (x >= 1.0 && x < 2.0)
        return x - 1.0;
    if (x > -2.0 && x < -1.0)
        return x + 1.0;
    return fmod(x, 1.0);
}

static inline int dclang_phase_step(double *memory, size_t cells, double step,
                                    double base, double count_addr, double *old)
{
    if (!(count_addr >= 0 && count_addr < cells))
        return 0;
    double *count = memory + (size_t)count_addr;
    double address = base + *count;
    if (!(address >= 0 && address < cells))
        return 0;
    double *phase = memory + (size_t)address;
    *old = *phase;
    double next = dclang_unit_wrap(*old + step);
    const double drift = 1.0 / 2147483647.0;
    *phase = next <= drift || next >= 1.0 - drift ? 0.0 : next;
    *count += 1.0;
    return 1;
}

static inline int dclang_fm_sample(double *memory, size_t cells, double carrier_step,
                                   double modulator_step, double index, double base,
                                   double count, double *sample)
{
    double modulator_phase, carrier_phase;
    if (!dclang_phase_step(memory, cells, modulator_step, base, count, &modulator_phase))
        return 0;
    /* Preserve the VM's separate multiply and add rounding. */
    volatile double modulation = sin(modulator_phase * DCLANG_TWO_PI) * index;
    if (!dclang_phase_step(memory, cells, carrier_step, base, count, &carrier_phase))
        return 0;
    *sample = sin(dclang_unit_wrap(carrier_phase + modulation) * DCLANG_TWO_PI);
    return 1;
}

static inline int dclang_sineloop_sample(double *memory, size_t cells, double feedback_addr,
                                         double step, double feedback, double base,
                                         double count, double *sample)
{
    if (!(feedback_addr >= 0 && feedback_addr < cells))
        return 0;
    double *previous = memory + (size_t)feedback_addr;
    volatile double modulation = (*previous * feedback) * (1.0 / DCLANG_TWO_PI);
    double phase;
    if (!dclang_phase_step(memory, cells, step, base, count, &phase))
        return 0;
    *sample = sin((modulation + phase) * DCLANG_TWO_PI);
    *previous = *sample;
    return 1;
}

static inline int dclang_tri_sample(double *memory, size_t cells, double step,
                                    double base, double count, double *sample)
{
    double phase;
    if (!dclang_phase_step(memory, cells, step, base, count, &phase))
        return 0;
    *sample = fabs(dclang_unit_wrap(phase + 0.75) - 0.5) * 4.0 - 1.0;
    return 1;
}

static inline int dclang_sawtooth_sample(double *memory, size_t cells, double step,
                                         double base, double count, double *sample)
{
    double phase;
    if (!dclang_phase_step(memory, cells, step, base, count, &phase))
        return 0;
    *sample = dclang_unit_wrap(phase + 0.5) * 2.0 - 1.0;
    return 1;
}

#endif
