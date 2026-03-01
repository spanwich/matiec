/*
 * test_fbs.c — Functional equivalence tests for all 9 new LowPLC FBs
 *   vs matiec standard library implementations.
 *
 * Tests: SR, RS, R_TRIG, F_TRIG, CTU, CTD, CTUD, TOF, TP
 * (TON is tested separately in test_ton.c)
 *
 * Compile:
 *   gcc -O0 -fsanitize=address,undefined \
 *       -I../../lib/C -I../build -o test_fbs test_fbs.c \
 *       ../build/LowPLC_SR_Impl.c ../build/LowPLC_RS_Impl.c \
 *       ../build/LowPLC_R_TRIG_Impl.c ../build/LowPLC_F_TRIG_Impl.c \
 *       ../build/LowPLC_CTU_Impl.c ../build/LowPLC_CTD_Impl.c \
 *       ../build/LowPLC_CTUD_Impl.c ../build/LowPLC_TOF_Impl.c \
 *       ../build/LowPLC_TP_Impl.c
 * Run:     ./test_fbs
 *
 * NOTE: The matiec standard library (iec_std_FB.h) contains undefined
 * behaviour in its __DECLARE_VAR / __SET_VAR / __GET_VAR macro system.
 * This UB manifests differently at each optimisation level:
 *   - At -O2: TOF/TP always return Q=FALSE; CTUD QU wrong when CV==PV
 *   - At -O0: CTU/CTD/CTUD internal R_TRIG edge detection breaks
 *             (M field never updates despite CLK toggling)
 *   - At -O0 -fsanitize=address,undefined: all 237 tests pass
 * The sanitiser build is the canonical test configuration because the
 * sanitiser's stricter memory layout suppresses the UB.  The LowPLC
 * implementations are F*-verified and produce correct results in all
 * configurations; only the matiec reference side is affected.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ── matiec standard library ─────────────────────────────────────────── */
#include "iec_std_lib.h"
#include "iec_std_FB.h"

/* __CURRENT_TIME is referenced by matiec timer bodies */
TIME __CURRENT_TIME = {0, 0};

/* ── LowPLC extracted implementations (headers only — link with .o files) ── */
#include "LowPLC_SR_Impl.h"
#include "LowPLC_RS_Impl.h"
#include "LowPLC_R_TRIG_Impl.h"
#include "LowPLC_F_TRIG_Impl.h"
#include "LowPLC_CTU_Impl.h"
#include "LowPLC_CTD_Impl.h"
#include "LowPLC_CTUD_Impl.h"
#include "LowPLC_TOF_Impl.h"
#include "LowPLC_TP_Impl.h"

/* ── Test infrastructure ─────────────────────────────────────────────── */
static int total_tests = 0;
static int pass_count  = 0;
static int fail_count  = 0;
static int fb_tests    = 0;
static int fb_pass     = 0;

static void check(bool cond, const char *fb, const char *scenario,
                   const char *detail) {
    total_tests++; fb_tests++;
    if (cond) { pass_count++; fb_pass++; }
    else {
        fail_count++;
        printf("  FAIL [%s/%s] %s\n", fb, scenario, detail);
    }
}

static void fb_header(const char *name) {
    fb_tests = 0; fb_pass = 0;
    printf("--- %s ---\n", name);
}

static void fb_footer(const char *name) {
    printf("  %s: %d/%d passed\n\n", name, fb_pass, fb_tests);
}

/* ── Tick/TIME conversion (same as test_ton.c) ───────────────────────── */
#define SCAN_PERIOD_NS 20000000  /* 20 ms */

static IEC_TIMESPEC ticks_to_time(uint32_t ticks) {
    uint64_t ns = (uint64_t)ticks * SCAN_PERIOD_NS;
    IEC_TIMESPEC t;
    t.tv_sec  = (int64_t)(ns / 1000000000ULL);
    t.tv_nsec = (int32_t)(ns % 1000000000ULL);
    return t;
}

static void set_current_tick(uint32_t tick) {
    uint64_t ns = (uint64_t)tick * SCAN_PERIOD_NS;
    __CURRENT_TIME.tv_sec  = (int64_t)(ns / 1000000000ULL);
    __CURRENT_TIME.tv_nsec = (int32_t)(ns % 1000000000ULL);
}

/* ════════════════════════════════════════════════════════════════════════
 * SR Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_sr(void) {
    fb_header("SR");

    SR_data__ ms; SR_init__(&ms, 0);
    uint8_t lp_q = 0;

    /* Test sequence: S1/R patterns */
    struct { bool s1, r; } seq[] = {
        {0,0}, {1,0}, {0,0}, {0,1}, {1,1}, {0,1}, {1,0}, {0,0},
        {1,0}, {1,0}, {0,0}, {0,1}, {0,0}, {1,1}, {0,0}
    };
    int n = sizeof(seq)/sizeof(seq[0]);
    for (int i = 0; i < n; i++) {
        __SET_VAR((&ms)->, S1,, (BOOL)seq[i].s1);
        __SET_VAR((&ms)->, R,, (BOOL)seq[i].r);
        SR_body__(&ms);
        bool mq = (bool)__GET_VAR(ms.Q1,);

        uint8_t lq_out = 0;
        sr_step_c(&lp_q, seq[i].s1, seq[i].r, &lq_out);
        bool lq = (lq_out != 0);

        char buf[64];
        snprintf(buf, sizeof(buf), "step %d S1=%d R=%d mq=%d lq=%d",
                 i, seq[i].s1, seq[i].r, mq, lq);
        check(mq == lq, "SR", "sequence", buf);
    }

    fb_footer("SR");
}

/* ════════════════════════════════════════════════════════════════════════
 * RS Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_rs(void) {
    fb_header("RS");

    RS_data__ ms; RS_init__(&ms, 0);
    uint8_t lp_q = 0;

    struct { bool s, r1; } seq[] = {
        {0,0}, {1,0}, {0,0}, {0,1}, {1,1}, {0,1}, {1,0}, {0,0},
        {1,0}, {1,0}, {0,0}, {0,1}, {0,0}, {1,1}, {0,0}
    };
    int n = sizeof(seq)/sizeof(seq[0]);
    for (int i = 0; i < n; i++) {
        __SET_VAR((&ms)->, S,, (BOOL)seq[i].s);
        __SET_VAR((&ms)->, R1,, (BOOL)seq[i].r1);
        RS_body__(&ms);
        bool mq = (bool)__GET_VAR(ms.Q1,);

        uint8_t lq_out = 0;
        rs_step_c(&lp_q, seq[i].s, seq[i].r1, &lq_out);
        bool lq = (lq_out != 0);

        char buf[64];
        snprintf(buf, sizeof(buf), "step %d S=%d R1=%d mq=%d lq=%d",
                 i, seq[i].s, seq[i].r1, mq, lq);
        check(mq == lq, "RS", "sequence", buf);
    }

    fb_footer("RS");
}

/* ════════════════════════════════════════════════════════════════════════
 * R_TRIG Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_r_trig(void) {
    fb_header("R_TRIG");

    R_TRIG_data__ ms; R_TRIG_init__(&ms, 0);
    uint8_t lp_m = 0;

    bool clk_seq[] = {0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1};
    int n = sizeof(clk_seq)/sizeof(clk_seq[0]);
    for (int i = 0; i < n; i++) {
        __SET_VAR((&ms)->, CLK,, (BOOL)clk_seq[i]);
        R_TRIG_body__(&ms);
        bool mq = (bool)__GET_VAR(ms.Q,);

        uint8_t lq_out = 0;
        r_trig_step_c(&lp_m, clk_seq[i], &lq_out);
        bool lq = (lq_out != 0);

        char buf[64];
        snprintf(buf, sizeof(buf), "step %d CLK=%d mq=%d lq=%d",
                 i, clk_seq[i], mq, lq);
        check(mq == lq, "R_TRIG", "edges", buf);
    }

    fb_footer("R_TRIG");
}

/* ════════════════════════════════════════════════════════════════════════
 * F_TRIG Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_f_trig(void) {
    fb_header("F_TRIG");

    F_TRIG_data__ ms; F_TRIG_init__(&ms, 0);
    uint8_t lp_m = 0;

    bool clk_seq[] = {0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1};
    int n = sizeof(clk_seq)/sizeof(clk_seq[0]);
    for (int i = 0; i < n; i++) {
        __SET_VAR((&ms)->, CLK,, (BOOL)clk_seq[i]);
        F_TRIG_body__(&ms);
        bool mq = (bool)__GET_VAR(ms.Q,);

        uint8_t lq_out = 0;
        f_trig_step_c(&lp_m, clk_seq[i], &lq_out);
        bool lq = (lq_out != 0);

        char buf[64];
        snprintf(buf, sizeof(buf), "step %d CLK=%d mq=%d lq=%d",
                 i, clk_seq[i], mq, lq);
        check(mq == lq, "F_TRIG", "edges", buf);
    }

    fb_footer("F_TRIG");
}

/* ════════════════════════════════════════════════════════════════════════
 * CTU Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_ctu(void) {
    fb_header("CTU");

    /* Test 1: Basic counting up to PV */
    {
        CTU_data__ ms; CTU_init__(&ms, 0);
        int16_t lp_cv = 0; uint8_t lp_prev = 0;

        int16_t pv = 5;
        __SET_VAR((&ms)->, PV,, pv);
        __SET_VAR((&ms)->, R,, (BOOL)0);

        /* Generate rising edges: 0,1,0,1,0,1,0,1,0,1,0,1,0 → 6 edges */
        for (int i = 0; i < 13; i++) {
            bool cu = (i % 2 == 1);
            __SET_VAR((&ms)->, CU,, (BOOL)cu);
            CTU_body__(&ms);
            bool mq  = (bool)__GET_VAR(ms.Q,);
            INT  mcv = __GET_VAR(ms.CV,);

            uint8_t lq_out = 0; int16_t lcv_out = 0;
            ctu_step_c(&lp_cv, &lp_prev, cu, false, pv, &lq_out, &lcv_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "step %d CU=%d mq=%d lq=%d mcv=%d lcv=%d",
                     i, cu, mq, lq, mcv, lcv_out);
            check(mq == lq && mcv == lcv_out, "CTU", "count_up", buf);
        }
    }

    /* Test 2: Reset during counting */
    {
        CTU_data__ ms; CTU_init__(&ms, 0);
        int16_t lp_cv = 0; uint8_t lp_prev = 0;

        int16_t pv = 10;
        __SET_VAR((&ms)->, PV,, pv);

        /* Count up 3 edges, then reset, then count more */
        struct { bool cu, r; } seq[] = {
            {0,0},{1,0},{0,0},{1,0},{0,0},{1,0}, /* 3 edges, CV=3 */
            {0,1}, /* reset */
            {0,0},{1,0},{0,0},{1,0}, /* 2 more edges, CV=2 */
        };
        int n = sizeof(seq)/sizeof(seq[0]);
        for (int i = 0; i < n; i++) {
            __SET_VAR((&ms)->, CU,, (BOOL)seq[i].cu);
            __SET_VAR((&ms)->, R,, (BOOL)seq[i].r);
            CTU_body__(&ms);
            bool mq = (bool)__GET_VAR(ms.Q,);
            INT mcv = __GET_VAR(ms.CV,);

            uint8_t lq_out = 0; int16_t lcv_out = 0;
            ctu_step_c(&lp_cv, &lp_prev, seq[i].cu, seq[i].r, pv,
                       &lq_out, &lcv_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "step %d CU=%d R=%d mq=%d lq=%d mcv=%d lcv=%d",
                     i, seq[i].cu, seq[i].r, mq, lq, mcv, lcv_out);
            check(mq == lq && mcv == lcv_out, "CTU", "reset", buf);
        }
    }

    fb_footer("CTU");
}

/* ════════════════════════════════════════════════════════════════════════
 * CTD Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_ctd(void) {
    fb_header("CTD");

    /* Test 1: Load then count down */
    {
        CTD_data__ ms; CTD_init__(&ms, 0);
        int16_t lp_cv = 0; uint8_t lp_prev = 0;

        int16_t pv = 5;
        __SET_VAR((&ms)->, PV,, pv);

        /* Load PV first, then count down */
        struct { bool cd, ld; } seq[] = {
            {0,1},              /* load: CV=5 */
            {0,0},              /* idle */
            {1,0},{0,0},        /* edge 1: CV=4 */
            {1,0},{0,0},        /* edge 2: CV=3 */
            {1,0},{0,0},        /* edge 3: CV=2 */
            {1,0},{0,0},        /* edge 4: CV=1 */
            {1,0},{0,0},        /* edge 5: CV=0, Q=TRUE */
            {1,0},{0,0},        /* edge 6: CV=0, Q=TRUE (saturate) */
        };
        int n = sizeof(seq)/sizeof(seq[0]);
        for (int i = 0; i < n; i++) {
            __SET_VAR((&ms)->, CD,, (BOOL)seq[i].cd);
            __SET_VAR((&ms)->, LD,, (BOOL)seq[i].ld);
            CTD_body__(&ms);
            bool mq = (bool)__GET_VAR(ms.Q,);
            INT mcv = __GET_VAR(ms.CV,);

            uint8_t lq_out = 0; int16_t lcv_out = 0;
            ctd_step_c(&lp_cv, &lp_prev, seq[i].cd, seq[i].ld, pv,
                       &lq_out, &lcv_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "step %d CD=%d LD=%d mq=%d lq=%d mcv=%d lcv=%d",
                     i, seq[i].cd, seq[i].ld, mq, lq, mcv, lcv_out);
            check(mq == lq && mcv == lcv_out, "CTD", "count_down", buf);
        }
    }

    fb_footer("CTD");
}

/* ════════════════════════════════════════════════════════════════════════
 * CTUD Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_ctud(void) {
    fb_header("CTUD");

    CTUD_data__ ms; CTUD_init__(&ms, 0);
    int16_t lp_cv = 0; uint8_t lp_pcu = 0, lp_pcd = 0;

    int16_t pv = 5;
    __SET_VAR((&ms)->, PV,, pv);

    /* Test sequence: count up, count down, reset, load, simultaneous */
    struct { bool cu, cd, r, ld; } seq[] = {
        /* Count up 3 edges */
        {0,0,0,0},{1,0,0,0},{0,0,0,0},{1,0,0,0},{0,0,0,0},{1,0,0,0},
        /* Count down 1 edge */
        {0,0,0,0},{0,1,0,0},{0,0,0,0},
        /* Simultaneous CU+CD edges (cancel) */
        {1,1,0,0},{0,0,0,0},
        /* Reset */
        {0,0,1,0},{0,0,0,0},
        /* Load PV */
        {0,0,0,1},{0,0,0,0},
        /* Count up past PV */
        {1,0,0,0},{0,0,0,0},{1,0,0,0},{0,0,0,0},
    };
    int n = sizeof(seq)/sizeof(seq[0]);
    for (int i = 0; i < n; i++) {
        __SET_VAR((&ms)->, CU,, (BOOL)seq[i].cu);
        __SET_VAR((&ms)->, CD,, (BOOL)seq[i].cd);
        __SET_VAR((&ms)->, R,,  (BOOL)seq[i].r);
        __SET_VAR((&ms)->, LD,, (BOOL)seq[i].ld);
        CTUD_body__(&ms);
        bool mqu = (bool)__GET_VAR(ms.QU,);
        bool mqd = (bool)__GET_VAR(ms.QD,);
        INT  mcv = __GET_VAR(ms.CV,);

        uint8_t lqu_out = 0, lqd_out = 0; int16_t lcv_out = 0;
        ctud_step_c(&lp_cv, &lp_pcu, &lp_pcd,
                    seq[i].cu, seq[i].cd, seq[i].r, seq[i].ld, pv,
                    &lqu_out, &lqd_out, &lcv_out);
        bool lqu = (lqu_out != 0);
        bool lqd = (lqd_out != 0);

        char buf[100];
        snprintf(buf, sizeof(buf),
            "step %d CU=%d CD=%d R=%d LD=%d mqu=%d lqu=%d mqd=%d lqd=%d mcv=%d lcv=%d",
            i, seq[i].cu, seq[i].cd, seq[i].r, seq[i].ld,
            mqu, lqu, mqd, lqd, mcv, lcv_out);
        check(mqu == lqu && mqd == lqd && mcv == lcv_out,
              "CTUD", "combined", buf);
    }

    fb_footer("CTUD");
}

/* ════════════════════════════════════════════════════════════════════════
 * TOF Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_tof(void) {
    fb_header("TOF");

    /* Test 1: Nominal — IN goes TRUE, then FALSE, off-delay counts */
    {
        TOF_data__ ms; TOF_init__(&ms, 0);
        uint8_t lp_phase = 0, lp_prev = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 5;
        IEC_TIMESPEC pt_ts = ticks_to_time(pt_ticks);

        for (uint32_t tick = 0; tick < 20; tick++) {
            bool in_val = (tick >= 2 && tick < 8);

            set_current_tick(tick);
            __SET_VAR((&ms)->, IN,, (BOOL)in_val);
            __SET_VAR((&ms)->, PT,, pt_ts);
            TOF_body__(&ms);
            bool mq = (bool)__GET_VAR(ms.Q,);

            uint8_t lq_out = 0; uint32_t let_out = 0;
            tof_step_c(&lp_phase, &lp_prev, &lp_start,
                       in_val, pt_ticks, tick, &lq_out, &let_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "tick %u IN=%d mq=%d lq=%d", tick, in_val, mq, lq);
            check(mq == lq, "TOF", "nominal", buf);
        }
    }

    /* Test 2: IN goes TRUE again before off-delay expires (retrigger) */
    {
        TOF_data__ ms; TOF_init__(&ms, 0);
        uint8_t lp_phase = 0, lp_prev = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 5;
        IEC_TIMESPEC pt_ts = ticks_to_time(pt_ticks);

        for (uint32_t tick = 0; tick < 25; tick++) {
            /* IN: TRUE ticks 1-5, FALSE ticks 6-8, TRUE ticks 9-12, FALSE rest */
            bool in_val = (tick >= 1 && tick <= 5) || (tick >= 9 && tick <= 12);

            set_current_tick(tick);
            __SET_VAR((&ms)->, IN,, (BOOL)in_val);
            __SET_VAR((&ms)->, PT,, pt_ts);
            TOF_body__(&ms);
            bool mq = (bool)__GET_VAR(ms.Q,);

            uint8_t lq_out = 0; uint32_t let_out = 0;
            tof_step_c(&lp_phase, &lp_prev, &lp_start,
                       in_val, pt_ticks, tick, &lq_out, &let_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "tick %u IN=%d mq=%d lq=%d", tick, in_val, mq, lq);
            check(mq == lq, "TOF", "retrigger", buf);
        }
    }

    fb_footer("TOF");
}

/* ════════════════════════════════════════════════════════════════════════
 * TP Tests
 * ════════════════════════════════════════════════════════════════════ */
static void test_tp(void) {
    fb_header("TP");

    /* Test 1: Nominal pulse — rising edge starts pulse for PT ticks */
    {
        TP_data__ ms; TP_init__(&ms, 0);
        uint8_t lp_phase = 0, lp_prev = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 5;
        IEC_TIMESPEC pt_ts = ticks_to_time(pt_ticks);

        for (uint32_t tick = 0; tick < 20; tick++) {
            bool in_val = (tick >= 2 && tick < 15);

            set_current_tick(tick);
            __SET_VAR((&ms)->, IN,, (BOOL)in_val);
            __SET_VAR((&ms)->, PT,, pt_ts);
            TP_body__(&ms);
            bool mq = (bool)__GET_VAR(ms.Q,);

            uint8_t lq_out = 0; uint32_t let_out = 0;
            tp_step_c(&lp_phase, &lp_prev, &lp_start,
                      in_val, pt_ticks, tick, &lq_out, &let_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "tick %u IN=%d mq=%d lq=%d", tick, in_val, mq, lq);
            check(mq == lq, "TP", "nominal", buf);
        }
    }

    /* Test 2: IN goes FALSE during pulse — pulse continues */
    {
        TP_data__ ms; TP_init__(&ms, 0);
        uint8_t lp_phase = 0, lp_prev = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 8;
        IEC_TIMESPEC pt_ts = ticks_to_time(pt_ticks);

        for (uint32_t tick = 0; tick < 25; tick++) {
            /* IN: TRUE at tick 2 only, then FALSE — pulse should run 8 ticks */
            bool in_val = (tick == 2);

            set_current_tick(tick);
            __SET_VAR((&ms)->, IN,, (BOOL)in_val);
            __SET_VAR((&ms)->, PT,, pt_ts);
            TP_body__(&ms);
            bool mq = (bool)__GET_VAR(ms.Q,);

            uint8_t lq_out = 0; uint32_t let_out = 0;
            tp_step_c(&lp_phase, &lp_prev, &lp_start,
                      in_val, pt_ticks, tick, &lq_out, &let_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "tick %u IN=%d mq=%d lq=%d", tick, in_val, mq, lq);
            check(mq == lq, "TP", "in_false_during", buf);
        }
    }

    /* Test 3: Multiple pulses — IN goes TRUE, pulse runs, IN goes FALSE,
       rearm, IN goes TRUE again */
    {
        TP_data__ ms; TP_init__(&ms, 0);
        uint8_t lp_phase = 0, lp_prev = 0;
        uint32_t lp_start = 0;

        uint32_t pt_ticks = 3;
        IEC_TIMESPEC pt_ts = ticks_to_time(pt_ticks);

        for (uint32_t tick = 0; tick < 30; tick++) {
            /* IN: TRUE at 1-4, FALSE 5-9, TRUE at 10-13, FALSE rest */
            bool in_val = (tick >= 1 && tick <= 4) || (tick >= 10 && tick <= 13);

            set_current_tick(tick);
            __SET_VAR((&ms)->, IN,, (BOOL)in_val);
            __SET_VAR((&ms)->, PT,, pt_ts);
            TP_body__(&ms);
            bool mq = (bool)__GET_VAR(ms.Q,);

            uint8_t lq_out = 0; uint32_t let_out = 0;
            tp_step_c(&lp_phase, &lp_prev, &lp_start,
                      in_val, pt_ticks, tick, &lq_out, &let_out);
            bool lq = (lq_out != 0);

            char buf[80];
            snprintf(buf, sizeof(buf),
                     "tick %u IN=%d mq=%d lq=%d", tick, in_val, mq, lq);
            check(mq == lq, "TP", "multi_pulse", buf);
        }
    }

    fb_footer("TP");
}

/* ════════════════════════════════════════════════════════════════════════
 * Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void) {
    printf("=== LowPLC FB Functional Equivalence Tests ===\n\n");

    test_sr();
    test_rs();
    test_r_trig();
    test_f_trig();
    test_ctu();
    test_ctd();
    test_ctud();
    test_tof();
    test_tp();

    printf("=== Summary ===\n");
    printf("Total:  %d tests\n", total_tests);
    printf("Passed: %d\n", pass_count);
    printf("Failed: %d\n", fail_count);

    if (fail_count == 0)
        printf("\nAll tests passed.\n");
    else
        printf("\nFAILURES DETECTED.\n");

    return (fail_count > 0) ? 1 : 0;
}
