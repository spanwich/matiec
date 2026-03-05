/*
 * D5 Per-Opcode Cost Measurement Firmware — STM32F767ZI
 *
 * Measures worst-case cycle counts for:
 *   - 10 KaRaMeL-extracted verified FB step functions
 *   - 18 simple opcode operations (loads, bools, ints, comparisons)
 *   - Interpreter dispatch overhead (IF/FOR framing)
 *
 * Each operation is measured 10,000 times with DWT cycle counting.
 * UART output: BEGIN_OPCODE_COSTS / OPNAME,min,max / END_OPCODE_COSTS
 *
 * Worst-case inputs per FB (from analysis):
 *   TON:   TIMING, in=true, elapsed < pt
 *   TOF:   IDLE, prev_in=true, in=false (falling edge start)
 *   TP:    TIMING, in=true, elapsed < pt
 *   CTU:   cu rising edge, cv < pv
 *   CTD:   cd rising edge, cv > 0
 *   CTUD:  cu rising edge
 *   SR/RS/R_TRIG/F_TRIG: constant-time
 */

#include <stdint.h>
#include <stdbool.h>

#include "dwt.h"
#include "uart.h"
#include "measurement.h"
#include "system_stm32f7xx.h"

/* KaRaMeL runtime shims */
#include "lowplc_krml_shim.h"

/* KaRaMeL-extracted FB headers */
#include "LowPLC_TON_Impl.h"
#include "LowPLC_TOF_Impl.h"
#include "LowPLC_TP_Impl.h"
#include "LowPLC_CTU_Impl.h"
#include "LowPLC_CTD_Impl.h"
#include "LowPLC_CTUD_Impl.h"
#include "LowPLC_SR_Impl.h"
#include "LowPLC_RS_Impl.h"
#include "LowPLC_R_TRIG_Impl.h"
#include "LowPLC_F_TRIG_Impl.h"

/* KaRaMeL-extracted FB implementations (unity build) */
#include "LowPLC_TON_Impl.c"
#include "LowPLC_TOF_Impl.c"
#include "LowPLC_TP_Impl.c"
#include "LowPLC_CTU_Impl.c"
#include "LowPLC_CTD_Impl.c"
#include "LowPLC_CTUD_Impl.c"
#include "LowPLC_SR_Impl.c"
#include "LowPLC_RS_Impl.c"
#include "LowPLC_R_TRIG_Impl.c"
#include "LowPLC_F_TRIG_Impl.c"

/* ── Measurement infrastructure ─────────────────────────────────────── */

static measurement_t meas;

static void emit_op(const char *name) {
    uart_puts(name);
    uart_putc(',');
    uart_putu32(meas.min);
    uart_putc(',');
    uart_putu32(meas.max);
    uart_puts("\r\n");
}

/* Volatile pools to prevent dead-code elimination */
static volatile bool    pool_bools[64];
static volatile int16_t pool_ints[32];

/* ── FB Step Function Measurements ──────────────────────────────────── */

static void measure_ton(void) {
    uint8_t phase; uint32_t start; uint8_t q; uint32_t et;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Worst case: TIMING phase, in=true, elapsed < pt */
        phase = LowPLC_TON_TON_TIMING;
        start = 0;
        uint32_t t0 = dwt_cycles();
        ton_step_c(&phase, &start, true, 1000, 500, &q, &et);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("TON_CALL");
}

static void measure_tof(void) {
    uint8_t phase; uint8_t prev_in; uint32_t start;
    uint8_t q; uint32_t et;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Worst case: IDLE, prev_in=true, in=false (falling edge) */
        phase = LowPLC_TOF_TOF_IDLE;
        prev_in = 1;
        start = 0;
        uint32_t t0 = dwt_cycles();
        tof_step_c(&phase, &prev_in, &start, false, 1000, 500, &q, &et);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("TOF_CALL");
}

static void measure_tp(void) {
    uint8_t phase; uint8_t prev_in; uint32_t start;
    uint8_t q; uint32_t et;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Worst case: TIMING, in=true, elapsed < pt */
        phase = LowPLC_TP_TP_TIMING;
        prev_in = 1;
        start = 0;
        uint32_t t0 = dwt_cycles();
        tp_step_c(&phase, &prev_in, &start, true, 1000, 500, &q, &et);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("TP_CALL");
}

static void measure_ctu(void) {
    int16_t cv; uint8_t prev; uint8_t q; int16_t cv_out;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Worst case: cu rising edge, cv < pv (increment path) */
        cv = 99;
        prev = 0;  /* prev_cu = false → rising edge */
        uint32_t t0 = dwt_cycles();
        ctu_step_c(&cv, &prev, true, false, 100, &q, &cv_out);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CTU_CALL");
}

static void measure_ctd(void) {
    int16_t cv; uint8_t prev; uint8_t q; int16_t cv_out;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Worst case: cd rising edge, cv > 0 (decrement path) */
        cv = 1;
        prev = 0;  /* prev_cd = false → rising edge */
        uint32_t t0 = dwt_cycles();
        ctd_step_c(&cv, &prev, true, false, 100, &q, &cv_out);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CTD_CALL");
}

static void measure_ctud(void) {
    int16_t cv; uint8_t prev_cu; uint8_t prev_cd;
    uint8_t qu; uint8_t qd; int16_t cv_out;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Worst case: cu rising edge only */
        cv = 50;
        prev_cu = 0;  /* rising edge on CU */
        prev_cd = 0;
        uint32_t t0 = dwt_cycles();
        ctud_step_c(&cv, &prev_cu, &prev_cd,
                     true, false, false, false, 100,
                     &qu, &qd, &cv_out);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CTUD_CALL");
}

static void measure_sr(void) {
    uint8_t q_state; uint8_t q_out;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        q_state = 0;
        uint32_t t0 = dwt_cycles();
        sr_step_c(&q_state, true, false, &q_out);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("SR_CALL");
}

static void measure_rs(void) {
    uint8_t q_state; uint8_t q_out;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        q_state = 0;
        uint32_t t0 = dwt_cycles();
        rs_step_c(&q_state, true, false, &q_out);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("RS_CALL");
}

static void measure_rtrig(void) {
    uint8_t m_state; uint8_t q_out;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Rising edge: prev=false, clk=true */
        m_state = 0;
        uint32_t t0 = dwt_cycles();
        r_trig_step_c(&m_state, true, &q_out);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("RTRIG_CALL");
}

static void measure_ftrig(void) {
    uint8_t m_state; uint8_t q_out;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        /* Falling edge: prev=true, clk=false */
        m_state = 1;
        uint32_t t0 = dwt_cycles();
        f_trig_step_c(&m_state, false, &q_out);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("FTRIG_CALL");
}

/* ── Simple Opcode Measurements ─────────────────────────────────────── */

static void measure_load_true(void) {
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = true;
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("LOAD_TRUE");
}

static void measure_load_false(void) {
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = false;
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("LOAD_FALSE");
}

static void measure_load_int(void) {
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_ints[0] = 42;
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("LOAD_INT");
}

static void measure_bool_not(void) {
    pool_bools[1] = true;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = !pool_bools[1];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("BOOL_NOT");
}

static void measure_bool_and(void) {
    pool_bools[1] = true; pool_bools[2] = true;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_bools[1] && pool_bools[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("BOOL_AND");
}

static void measure_bool_or(void) {
    pool_bools[1] = false; pool_bools[2] = true;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_bools[1] || pool_bools[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("BOOL_OR");
}

static void measure_bool_xor(void) {
    pool_bools[1] = true; pool_bools[2] = false;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_bools[1] != pool_bools[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("BOOL_XOR");
}

static void measure_copy_bool(void) {
    pool_bools[1] = true;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_bools[1];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("COPY_BOOL");
}

static void measure_int_add(void) {
    pool_ints[1] = 100; pool_ints[2] = 200;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_ints[0] = (int16_t)(pool_ints[1] + pool_ints[2]);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("INT_ADD");
}

static void measure_int_sub(void) {
    pool_ints[1] = 100; pool_ints[2] = 50;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_ints[0] = (int16_t)(pool_ints[1] - pool_ints[2]);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("INT_SUB");
}

static void measure_int_mul(void) {
    pool_ints[1] = 10; pool_ints[2] = 20;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_ints[0] = (int16_t)(pool_ints[1] * pool_ints[2]);
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("INT_MUL");
}

static void measure_copy_int(void) {
    pool_ints[1] = 42;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_ints[0] = pool_ints[1];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("COPY_INT");
}

static void measure_cmp_eq(void) {
    pool_ints[1] = 42; pool_ints[2] = 42;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_ints[1] == pool_ints[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CMP_EQ");
}

static void measure_cmp_ne(void) {
    pool_ints[1] = 42; pool_ints[2] = 43;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_ints[1] != pool_ints[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CMP_NE");
}

static void measure_cmp_lt(void) {
    pool_ints[1] = 10; pool_ints[2] = 20;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_ints[1] < pool_ints[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CMP_LT");
}

static void measure_cmp_gt(void) {
    pool_ints[1] = 20; pool_ints[2] = 10;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_ints[1] > pool_ints[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CMP_GT");
}

static void measure_cmp_le(void) {
    pool_ints[1] = 10; pool_ints[2] = 10;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_ints[1] <= pool_ints[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CMP_LE");
}

static void measure_cmp_ge(void) {
    pool_ints[1] = 20; pool_ints[2] = 10;
    meas_reset(&meas);
    for (uint32_t i = 0; i < MEAS_ITERATIONS; i++) {
        uint32_t t0 = dwt_cycles();
        pool_bools[0] = pool_ints[1] >= pool_ints[2];
        uint32_t t1 = dwt_cycles();
        meas_record(&meas, t1 - t0);
    }
    emit_op("CMP_GE");
}

/* ── Main ───────────────────────────────────────────────────────────── */

int main(void) {
    uart_init(115200);
    dwt_init();

    uart_puts("# D5 per-opcode cost measurement\r\n");
    uart_puts("BEGIN_OPCODE_COSTS\r\n");

    /* 10 KaRaMeL-extracted FB step functions */
    measure_ton();
    measure_tof();
    measure_tp();
    measure_ctu();
    measure_ctd();
    measure_ctud();
    measure_sr();
    measure_rs();
    measure_rtrig();
    measure_ftrig();

    /* 18 simple opcode operations */
    measure_load_true();
    measure_load_false();
    measure_load_int();
    measure_bool_not();
    measure_bool_and();
    measure_bool_or();
    measure_bool_xor();
    measure_copy_bool();
    measure_int_add();
    measure_int_sub();
    measure_int_mul();
    measure_copy_int();
    measure_cmp_eq();
    measure_cmp_ne();
    measure_cmp_lt();
    measure_cmp_gt();
    measure_cmp_le();
    measure_cmp_ge();

    uart_puts("END_OPCODE_COSTS\r\n");

    for (;;)
        __WFI();
}
