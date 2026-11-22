#ifndef DCLANG_DSP_H
#define DCLANG_DSP_H

#include "dclang_osc.h"
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

static inline int dclang_reverb_sample(double *memory, size_t cells, double left,
                                       double right, double base_arg, double cutoff,
                                       double feedback, double stride_arg, double sample_rate,
                                       double phase_base, double phase_count, double frame,
                                       double *wet_left, double *wet_right)
{
    // Offsets in make_reverb_struct in lib/dsp.dc.
    enum { LINES = 8, HEADER = 33, TIME = 9, FREQ = 17, DEPTH = 25 };
    if (cells < HEADER || !isfinite(frame) || frame < 0
        || !(sample_rate > 0 && isfinite(sample_rate))
        || !(base_arg >= 0 && base_arg <= cells - HEADER)
        || !(stride_arg >= 3 && stride_arg <= (cells - HEADER - base_arg) / LINES)) {
        return 0;
    }
    size_t base = (size_t)base_arg;
    size_t stride = (size_t)stride_arg;
    double inverse_rate = 1.0 / sample_rate;
    double pole = (1.0 - cutoff) * 0.5 + 0.5;
    double filtered[LINES];
    double cursor_length = 0.0;
    double write_cursor = 0.0;
    for (int line = 0; line < LINES; ++line) {
        size_t delay = base + HEADER + line * stride;
        double length = memory[delay];
        double position = memory[delay + 1];
        if (!(length >= 1 && length <= stride - 2)
            || !(position >= 0 && position < length)) {
            return 0;
        }
        double phase;
        volatile double step = memory[base + FREQ + line] * inverse_rate;
        if (!dclang_phase_step(memory, cells, step, phase_base, phase_count, &phase)) {
            return 0;
        }
        volatile double modulation = sin(phase * DCLANG_TWO_PI) * memory[base + DEPTH + line];
        double seconds = memory[base + TIME + line] + modulation;
        volatile double distance = seconds * sample_rate;
        double delayed;
        if (!dclang_delay_sample(memory, cells, delay, distance, &delayed)) {
            return 0;
        }
        size_t filter = base + 1 + line;
        double signal = delayed * feedback;
        volatile double feedforward = signal * (1.0 - pole);
        volatile double previous = memory[filter] * pole;
        filtered[line] = feedforward + previous;
        memory[filter] = filtered[line];
        double input = line % 2 ? right : left;
        memory[delay + 2 + (size_t)position] =
            filtered[line] * -1.0 + (memory[base] + input);
        // All eight lines normally use the same length. Derive the cursor
        // from the frame so skipped frames and separate instances still work.
        if (length != cursor_length) {
            write_cursor = frame < length ? frame : fmod(frame, length);
            cursor_length = length;
        }
        memory[delay + 1] = write_cursor;
    }
    double junction = filtered[0];
    for (int line = 1; line < LINES; ++line) {
        junction = filtered[line] + junction;
    }
    memory[base] = junction * 0.25;
    *wet_left = (filtered[6] + (filtered[4] + (filtered[2] + filtered[0]))) * 0.25;
    *wet_right = (filtered[7] + (filtered[5] + (filtered[3] + filtered[1]))) * 0.25;
    return 1;
}

#endif
