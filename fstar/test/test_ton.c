/*
 * test_ton.c — Functional equivalence test:
 *   LowPLC extracted ton_step_c  vs  matiec TON_body__
 *
 * Runs both implementations on identical input sequences and compares
 * the Q (timer output) at every scan cycle. TIME/tick conversion:
 *   tick N → __CURRENT_TIME = {0, N * scan_period_ns}
 *   pt_ticks P → PT = {0, P * scan_period_ns}
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

/* ── matiec TON ──────────────────────────────────────────────────────── */
/* We need the matiec accessor macros and TON implementation. */
#include "iec_std_lib.h"
#include "iec_std_FB.h"

/* __CURRENT_TIME is referenced by matiec TON_body__ */
TIME __CURRENT_TIME = {0, 0};

/* ── LowPLC extracted TON ────────────────────────────────────────────── */
#include "LowPLC_TON_Impl.h"

/* Inline the extracted implementation */
#include "LowPLC_TON_Impl.c"

/* ── Test infrastructure ─────────────────────────────────────────────── */

#define SCAN_PERIOD_NS 20000000  /* 20 ms scan period */

static int total_tests = 0;
static int pass_count  = 0;
static int fail_count  = 0;

static void test_scan_cycle(
    TON_data__   *matiec_state,
    uint8_t      *lowplc_phase,
    uint32_t     *lowplc_start,
    bool          in_val,
    uint32_t      pt_ticks,
    uint32_t      current_tick,
    const char   *scenario)
{
    /* ── Run matiec TON ──────────────────────────────────────────────── */
    __SET_VAR(matiec_state->, IN,, (BOOL)in_val);

    /* PT = pt_ticks * scan_period (in TIME format) */
    uint64_t pt_ns = (uint64_t)pt_ticks * SCAN_PERIOD_NS;
    IEC_TIMESPEC pt_ts;
    pt_ts.tv_sec  = (int64_t)(pt_ns / 1000000000ULL);
    pt_ts.tv_nsec = (int32_t)(pt_ns % 1000000000ULL);
    __SET_VAR(matiec_state->, PT,, pt_ts);

    /* Set __CURRENT_TIME = current_tick * scan_period */
    uint64_t cur_ns = (uint64_t)current_tick * SCAN_PERIOD_NS;
    __CURRENT_TIME.tv_sec  = (int64_t)(cur_ns / 1000000000ULL);
    __CURRENT_TIME.tv_nsec = (int32_t)(cur_ns % 1000000000ULL);

    TON_body__(matiec_state);
    BOOL matiec_q = __GET_VAR(matiec_state->Q,);

    /* ── Run LowPLC TON ──────────────────────────────────────────────── */
    uint8_t  lowplc_q_val = 0;
    uint32_t lowplc_et    = 0;
    ton_step_c(lowplc_phase, lowplc_start,
               in_val, pt_ticks, current_tick,
               &lowplc_q_val, &lowplc_et);

    bool lowplc_q = (lowplc_q_val != 0);

    /* ── Compare ─────────────────────────────────────────────────────── */
    total_tests++;
    bool match = ((bool)matiec_q == lowplc_q);
    if (match) {
        pass_count++;
    } else {
        fail_count++;
        printf("FAIL %s: tick=%u IN=%d Q_matiec=%d Q_lowplc=%d\n",
               scenario, current_tick, (int)in_val,
               (int)matiec_q, (int)lowplc_q);
    }
}

int main(void) {
    printf("=== TON Functional Equivalence Test ===\n\n");

    /* ── Test 1: Nominal — IN goes TRUE, timer runs, Q goes TRUE ──── */
    {
        TON_data__ matiec_s;
        TON_init__(&matiec_s, 0);

        uint8_t  lp_phase = 0;   /* TON_IDLE */
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 5;   /* T#100ms at 20ms scan = 5 ticks */

        printf("Test 1: Nominal timing (pt=5 ticks, 20 scans)\n");
        for (uint32_t tick = 0; tick < 20; tick++) {
            bool in_val = (tick >= 2 && tick < 15);
            test_scan_cycle(&matiec_s, &lp_phase, &lp_start,
                           in_val, pt_ticks, tick, "nominal");
        }
        printf("  nominal: %d tests\n", 20);
    }

    /* ── Test 2: Bouncing input — resets timer each cycle ────────── */
    {
        TON_data__ matiec_s;
        TON_init__(&matiec_s, 0);

        uint8_t  lp_phase = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 5;

        printf("Test 2: Bouncing input (alternating TRUE/FALSE)\n");
        for (uint32_t tick = 0; tick < 10; tick++) {
            bool in_val = (tick % 2 == 0);
            test_scan_cycle(&matiec_s, &lp_phase, &lp_start,
                           in_val, pt_ticks, tick, "bouncing");
        }
        printf("  bouncing: %d tests\n", 10);
    }

    /* ── Test 3: Immediate expiry — PT = 0 ticks ────────────────── */
    {
        TON_data__ matiec_s;
        TON_init__(&matiec_s, 0);

        uint8_t  lp_phase = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 0;

        printf("Test 3: Immediate expiry (PT=0)\n");
        for (uint32_t tick = 0; tick < 5; tick++) {
            bool in_val = (tick >= 1 && tick < 4);
            test_scan_cycle(&matiec_s, &lp_phase, &lp_start,
                           in_val, pt_ticks, tick, "immediate");
        }
        printf("  immediate: %d tests\n", 5);
    }

    /* ── Test 4: Long run — timer stays active across many scans ── */
    {
        TON_data__ matiec_s;
        TON_init__(&matiec_s, 0);

        uint8_t  lp_phase = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 50;

        printf("Test 4: Long run (pt=50 ticks, 100 scans)\n");
        for (uint32_t tick = 0; tick < 100; tick++) {
            bool in_val = (tick >= 5);   /* IN goes TRUE at tick 5, stays TRUE */
            test_scan_cycle(&matiec_s, &lp_phase, &lp_start,
                           in_val, pt_ticks, tick, "long_run");
        }
        printf("  long_run: %d tests\n", 100);
    }

    /* ── Test 5: Multiple activations — IN goes TRUE, timer expires,
       IN goes FALSE, then TRUE again ─────────────────────────────── */
    {
        TON_data__ matiec_s;
        TON_init__(&matiec_s, 0);

        uint8_t  lp_phase = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 3;

        printf("Test 5: Multiple activations (3 cycles)\n");
        /* Cycle 1: ticks 0-9, IN true at 1-6 */
        /* Cycle 2: ticks 10-19, IN true at 12-17 */
        /* Cycle 3: ticks 20-29, IN true at 22-27 */
        for (uint32_t tick = 0; tick < 30; tick++) {
            bool in_val = false;
            if (tick >= 1 && tick <= 6) in_val = true;
            if (tick >= 12 && tick <= 17) in_val = true;
            if (tick >= 22 && tick <= 27) in_val = true;
            test_scan_cycle(&matiec_s, &lp_phase, &lp_start,
                           in_val, pt_ticks, tick, "multi_act");
        }
        printf("  multi_act: %d tests\n", 30);
    }

    /* ── Results ──────────────────────────────────────────────────── */
    printf("\n=== Results ===\n");
    printf("Total:  %d tests\n", total_tests);
    printf("Passed: %d\n", pass_count);
    printf("Failed: %d\n", fail_count);

    if (fail_count == 0)
        printf("\nAll tests passed.\n");
    else
        printf("\nFAILURES DETECTED.\n");

    return (fail_count > 0) ? 1 : 0;
}
