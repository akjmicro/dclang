#ifndef DCLANG_OSC_H
#define DCLANG_OSC_H

#include "dclang_fast_math.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>

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

#define DCLANG_PHASE_SCALE 4294967296.0
#define DCLANG_OSC_STATE_CELLS 3

static inline int dclang_phase_step(DCLANG_CELL *memory, size_t cells, double hz,
                                    double sample_rate, double base,
                                    double count_addr, uint32_t *old)
{
    if (!(count_addr >= 0 && count_addr < cells) || !(sample_rate > 0.0))
        return 0;
    DCLANG_CELL *count_cell = memory + (size_t)count_addr;
    size_t count = (size_t)*count_cell;
    double address = base + (double)(count * DCLANG_OSC_STATE_CELLS);
    if (!(address >= 0 && address + 2.0 < cells))
        return 0;
    DCLANG_CELL *state = memory + (size_t)address;
    DCLANG_CELL hz_cell = dclang_float_to_cell((DCLANG_FLT)hz);
    if (state[1] != hz_cell) {
        state[1] = hz_cell;
        state[2] = (DCLANG_CELL)(uint32_t)llround(hz * DCLANG_PHASE_SCALE / sample_rate);
    }
    *old = (uint32_t)state[0];
    state[0] = (DCLANG_CELL)(uint32_t)(*old + (uint32_t)state[2]);
    *count_cell = (DCLANG_CELL)(count + 1);
    return 1;
}

static inline double dclang_phase_unit(uint32_t phase)
{
    return (double)phase / DCLANG_PHASE_SCALE;
}

static inline int dclang_phasor_sample(DCLANG_CELL *memory, size_t cells, double hz,
                                       double sample_rate, double base, double count,
                                       double *sample)
{
    uint32_t phase;
    if (!dclang_phase_step(memory, cells, hz, sample_rate, base, count, &phase))
        return 0;
    *sample = dclang_phase_unit(phase);
    return 1;
}

static inline int dclang_wavetable_lookup(DCLANG_CELL *memory, size_t cells,
                                          double table_addr, double table_length,
                                          uint32_t phase, double *sample)
{
    if (!(table_addr >= 0.0 && table_length >= 2.0 &&
          table_addr + table_length <= (double)cells))
        return 0;
    size_t length = (size_t)table_length;
    double position = dclang_phase_unit(phase) * (double)length;
    size_t index = (size_t)position;
    double fraction = position - (double)index;
    size_t next = index + 1 == length ? 0 : index + 1;
    double a = dclang_cell_to_float(memory[(size_t)table_addr + index]);
    double b = dclang_cell_to_float(memory[(size_t)table_addr + next]);
    *sample = a + (b - a) * fraction;
    return 1;
}

static inline int dclang_wavetable_sample(DCLANG_CELL *memory, size_t cells,
                                          double hz, double phase_offset, double sample_rate,
                                          double table_addr, double table_length,
                                          double base, double count, double *sample)
{
    uint32_t phase;
    uint32_t offset = (uint32_t)llround(phase_offset * DCLANG_PHASE_SCALE);
    return dclang_phase_step(memory, cells, hz, sample_rate, base, count, &phase) &&
           dclang_wavetable_lookup(memory, cells, table_addr, table_length,
                                   phase + offset, sample);
}

static inline int dclang_fm_sample(DCLANG_CELL *memory, size_t cells, double carrier_hz,
                                   double modulator_hz, double index, double sample_rate,
                                   double table_addr, double table_length, double base,
                                   double count, double *sample)
{
    uint32_t modulator_phase, carrier_phase;
    double modulator;
    if (!dclang_phase_step(memory, cells, modulator_hz, sample_rate, base, count, &modulator_phase) ||
        !dclang_wavetable_lookup(memory, cells, table_addr, table_length,
                                 modulator_phase, &modulator))
        return 0;
    if (!dclang_phase_step(memory, cells, carrier_hz, sample_rate, base, count, &carrier_phase))
        return 0;
    uint32_t offset = (uint32_t)llround(modulator * index * DCLANG_PHASE_SCALE);
    return dclang_wavetable_lookup(memory, cells, table_addr, table_length,
                                   carrier_phase + offset, sample);
}

static inline int dclang_sineloop_sample(DCLANG_CELL *memory, size_t cells, double feedback_addr,
                                         double hz, double feedback, double sample_rate,
                                         double table_addr, double table_length, double base,
                                         double count, double *sample)
{
    if (!(feedback_addr >= 0 && feedback_addr < cells))
        return 0;
    DCLANG_CELL *previous = memory + (size_t)feedback_addr;
    uint32_t phase;
    if (!dclang_phase_step(memory, cells, hz, sample_rate, base, count, &phase))
        return 0;
    uint32_t offset = (uint32_t)llround(dclang_cell_to_float(*previous) * feedback *
                                        (DCLANG_PHASE_SCALE / DCLANG_TWO_PI));
    if (!dclang_wavetable_lookup(memory, cells, table_addr, table_length,
                                 phase + offset, sample))
        return 0;
    *previous = dclang_float_to_cell(*sample);
    return 1;
}

static inline int dclang_tri_sample(DCLANG_CELL *memory, size_t cells, double hz,
                                    double sample_rate, double base, double count, double *sample)
{
    uint32_t raw_phase;
    if (!dclang_phase_step(memory, cells, hz, sample_rate, base, count, &raw_phase))
        return 0;
    double phase = dclang_phase_unit(raw_phase);
    *sample = fabs(dclang_unit_wrap(phase + 0.75) - 0.5) * 4.0 - 1.0;
    return 1;
}

static inline int dclang_sawtooth_sample(DCLANG_CELL *memory, size_t cells, double hz,
                                         double sample_rate, double base, double count, double *sample)
{
    uint32_t raw_phase;
    if (!dclang_phase_step(memory, cells, hz, sample_rate, base, count, &raw_phase))
        return 0;
    double phase = dclang_phase_unit(raw_phase);
    *sample = dclang_unit_wrap(phase + 0.5) * 2.0 - 1.0;
    return 1;
}

#endif
