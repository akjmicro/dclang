#ifndef DCLANG_DSP_H
#define DCLANG_DSP_H

#include "dclang_osc.h"
#include "dclang_phiverb.h"
#include <stdint.h>

static inline int dclang_port_sample(double *memory, size_t cells, double signal,
                                     double address, double rise, double fall, double *sample)
{
    if (!(address >= 0 && address < cells))
        return 0;
    double *state = memory + (size_t)address;
    double delta = signal - *state;
    // Preserve the VM's separate multiply and add rounding.
    volatile double adjustment = delta * (delta > 0.0 ? rise : fall);
    *sample = adjustment + *state;
    *state = *sample;
    return 1;
}

static inline int dclang_delay_sample(double *memory, size_t cells, double delay_addr,
                                      double distance, double *sample)
{
    if (cells < 3 || !(delay_addr >= 0 && delay_addr <= cells - 2))
        return 0;
    size_t delay = (size_t)delay_addr;
    double length = memory[delay];
    if (!(length >= 1 && length <= cells - delay - 2)) {
        return 0;
    }
    double position = memory[delay + 1] - distance;
    size_t lower, upper;
    double fraction;
    if (length <= INT32_MAX / 2 && position >= -length && position < 2 * length
        && length == (int32_t)length) {
        // Derive both taps from a single truncation. The interpolation
        // fraction is signed, even when the taps wrap around cell zero.
        int32_t ring = (int32_t)length;
        int32_t whole = (int32_t)position;
        fraction = position - whole;
        int32_t tap = whole - (fraction < 0.0);
        if (tap < 0)
            tap += ring;
        else if (tap >= ring)
            tap -= ring;
        lower = (size_t)tap;
        if (fraction == 0.0) {
            upper = lower;
            fraction = copysign(0.0, position);
        } else {
            upper = tap + 1 == ring ? 0 : (size_t)(tap + 1);
        }
    } else {
        // Wide excursions and user-defined fractional lengths retain the
        // original independent wrap for each tap.
        double low = fmod(fmod(floor(position), length) + length, length);
        double high = fmod(fmod(ceil(position), length) + length, length);
        if (!isfinite(low) || !isfinite(high))
            return 0;
        lower = (size_t)low;
        upper = (size_t)high;
        fraction = fmod(position, 1.0);
    }
    double first = memory[delay + 2 + lower];
    double second = memory[delay + 2 + upper];
    // Match the interpreter's separate multiply and add rounding.
    volatile double interpolated = (second - first) * fraction;
    *sample = interpolated + first;
    return 1;
}

#define DCLANG_PHIVERB_CELLS \
    ((sizeof(dclang_phiverb) + sizeof(double) - 1) / sizeof(double))

static inline int dclang_reverb_sample(double *memory, size_t cells, double left,
                                       double right, double base_arg, double cutoff,
                                       double feedback, double sample_rate,
                                       double *wet_left, double *wet_right)
{
    if (cells < DCLANG_PHIVERB_CELLS
        || !(base_arg >= 0.0 && base_arg <= cells - DCLANG_PHIVERB_CELLS)
        || !(sample_rate >= 50.0 && sample_rate <= 192000.0))
        return 0;
    size_t base = (size_t)base_arg;
    int rate = (int)sample_rate;
    dclang_phiverb *state = (dclang_phiverb *)(void *)(memory + base);
    if (state->fSampleRate != rate)
        initdclang_phiverb(state, rate);
    if (cutoff < 0.0) cutoff = 0.0;
    if (cutoff > 1.0) cutoff = 1.0;
    if (feedback < 0.0) feedback = 0.0;
    if (feedback > 0.999) feedback = 0.999;
    state->fHslider0 = (float)cutoff;
    state->fHslider1 = (float)feedback;
    float input_left = (float)left;
    float input_right = (float)right;
    float output_left, output_right;
    float *inputs[] = { &input_left, &input_right };
    float *outputs[] = { &output_left, &output_right };
    computedclang_phiverb(state, 1, inputs, outputs);
    *wet_left = output_left;
    *wet_right = output_right;
    return 1;
}

#endif
