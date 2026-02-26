/*
 * measurement.h — DWT cycle-count histogram and statistics
 *
 * Configuration: 10,000 iterations, 256 histogram bins, 10 cycles/bin.
 * Bin i covers [i*10, (i+1)*10) cycles.  Anything >= 2560 cycles goes into
 * the overflow bin (index 255).
 */
#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <stdint.h>

#define MEAS_ITERATIONS   10000U
#define MEAS_HIST_BINS    256U
#define MEAS_CYCLES_PER_BIN 10U

typedef struct {
    uint32_t min;
    uint32_t max;
    uint64_t sum;
    uint32_t count;
    uint32_t histogram[MEAS_HIST_BINS];
} measurement_t;

static inline void meas_reset(measurement_t *m) {
    m->min   = UINT32_MAX;
    m->max   = 0;
    m->sum   = 0;
    m->count = 0;
    __builtin_memset(m->histogram, 0, sizeof(m->histogram));
}

static inline void meas_record(measurement_t *m, uint32_t cycles) {
    if (cycles < m->min) m->min = cycles;
    if (cycles > m->max) m->max = cycles;
    m->sum += cycles;
    m->count++;

    uint32_t bin = cycles / MEAS_CYCLES_PER_BIN;
    if (bin >= MEAS_HIST_BINS)
        bin = MEAS_HIST_BINS - 1;
    m->histogram[bin]++;
}

#endif /* MEASUREMENT_H */
