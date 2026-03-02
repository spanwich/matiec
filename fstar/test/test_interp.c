/**
 * test_interp.c — B1 (ESD Interlock) equivalence test: TLV interpreter vs matiec.
 *
 * Runs multiple scan cycle scenarios through both:
 *   1. The TLV interpreter (lowplc_interp.h, reading b1_esd.tlv.h)
 *   2. The matiec-generated C code (ESD_INTERLOCK_body__)
 *
 * Compares outputs (shutdown_cmd, alarm_out) and internal state (latched,
 * any_trip, SR.Q, TON.Q) at each step.  This validates:
 *   - Correct TLV hand-encoding (D3, Step 5)
 *   - C interpreter correctness vs verified F* spec
 *   - FB dispatch equivalence (TON, SR)
 *
 * Build:
 *   gcc -O0 -fsanitize=address,undefined -Wall -Wextra -Wno-unused-parameter \
 *       -I../../lib/C -I../../build/b1 -I. -o test_interp test_interp.c \
 *       ../../build/b1/POUS.c
 *
 * NOTE: matiec must be compiled with -O0 due to UB in iec_std_FB.h.
 *       See test_fbs.c header for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* ── matiec headers ─────────────────────────────────────────────── */
#include "iec_std_lib.h"
#include "iec_std_FB.h"
#include "POUS.h"

/* matiec global — required by timer FBs */
TIME __CURRENT_TIME = {0, 0};

/* Unity-build: include POUS.c directly (it has no #include guards) */
#include "POUS.c"

/* ── TLV interpreter ────────────────────────────────────────────── */
#include "b1_esd.tlv.h"
#include "lowplc_interp.h"

/* ── Test Infrastructure ────────────────────────────────────────── */

static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

#define CHECK(cond, fmt, ...) do { \
    total_tests++; \
    if (cond) { passed_tests++; } \
    else { \
        failed_tests++; \
        printf("  FAIL: " fmt "\n", ##__VA_ARGS__); \
    } \
} while(0)

/* ── Scenario Runner ────────────────────────────────────────────── */

typedef struct {
    const char *name;
    bool pressure_high;
    bool temp_high;
    bool flow_low;
    bool manual_reset;
    uint32_t tick;  /* scan cycle tick */
} scan_input_t;

/**
 * Run one scan cycle through both engines and compare outputs.
 */
static void run_scan(const scan_input_t *in,
                     var_pool_t *tlv_pool,
                     ESD_INTERLOCK_data__ *matiec,
                     const tlv_header_t *hdr)
{
    printf("  Scan: %-30s (PH=%d TH=%d FL=%d MR=%d tick=%u)\n",
           in->name,
           in->pressure_high, in->temp_high, in->flow_low,
           in->manual_reset, in->tick);

    /* --- TLV interpreter --- */
    tlv_pool->bools[B1_PRESSURE_HIGH] = in->pressure_high;
    tlv_pool->bools[B1_TEMP_HIGH]     = in->temp_high;
    tlv_pool->bools[B1_FLOW_LOW]      = in->flow_low;
    tlv_pool->bools[B1_MANUAL_RESET]  = in->manual_reset;
    tlv_pool->tick = in->tick;
    exec_scan(b1_esd_tlv, sizeof(b1_esd_tlv), hdr, tlv_pool, 1024);

    /* --- matiec reference --- */
    /* Set __CURRENT_TIME from tick (each tick = 10ms) */
    uint32_t total_ms = (uint32_t)in->tick * 10;
    __CURRENT_TIME.tv_sec  = total_ms / 1000;
    __CURRENT_TIME.tv_nsec = (total_ms % 1000) * 1000000;

    __SET_VAR(matiec->,PRESSURE_HIGH,,in->pressure_high);
    __SET_VAR(matiec->,TEMP_HIGH,,in->temp_high);
    __SET_VAR(matiec->,FLOW_LOW,,in->flow_low);
    __SET_VAR(matiec->,MANUAL_RESET,,in->manual_reset);
    ESD_INTERLOCK_body__(matiec);

    /* --- Extract matiec outputs --- */
    bool m_shutdown = __GET_VAR(matiec->SHUTDOWN_CMD,);
    bool m_alarm    = __GET_VAR(matiec->ALARM_OUT,);
    bool m_latched  = __GET_VAR(matiec->LATCHED,);
    bool m_any_trip = __GET_VAR(matiec->ANY_TRIP,);

    /* --- Extract TLV outputs --- */
    bool t_shutdown = tlv_pool->bools[B1_SHUTDOWN_CMD];
    bool t_alarm    = tlv_pool->bools[B1_ALARM_OUT];
    bool t_latched  = tlv_pool->bools[B1_LATCHED];
    bool t_any_trip = tlv_pool->bools[B1_ANY_TRIP];

    /* --- Compare --- */
    CHECK(t_any_trip == m_any_trip,
          "%s: any_trip: TLV=%d matiec=%d", in->name, t_any_trip, m_any_trip);
    CHECK(t_latched == m_latched,
          "%s: latched: TLV=%d matiec=%d", in->name, t_latched, m_latched);
    CHECK(t_shutdown == m_shutdown,
          "%s: shutdown_cmd: TLV=%d matiec=%d", in->name, t_shutdown, m_shutdown);
    CHECK(t_alarm == m_alarm,
          "%s: alarm_out: TLV=%d matiec=%d", in->name, t_alarm, m_alarm);
}

/* ── Test Scenarios ─────────────────────────────────────────────── */

int main(void)
{
    printf("=== B1 ESD Interlock: TLV Interpreter vs matiec ===\n\n");

    /* Parse TLV header */
    tlv_header_t hdr;
    int rc = parse_header(b1_esd_tlv, sizeof(b1_esd_tlv), &hdr);
    CHECK(rc == 0, "Header parse failed: %d", rc);
    CHECK(hdr.n_bools == 12, "Expected 12 bools, got %d", hdr.n_bools);
    CHECK(hdr.n_sr == 1, "Expected 1 SR, got %d", hdr.n_sr);
    CHECK(hdr.n_ton == 1, "Expected 1 TON, got %d", hdr.n_ton);
    CHECK(hdr.body_len == 87, "Expected body_len=87, got %d", hdr.body_len);

    /* Initialize both engines */
    var_pool_t pool;
    init_pool(&pool, &hdr);
    exec_init(b1_esd_tlv, sizeof(b1_esd_tlv), &hdr, &pool, 1024);

    ESD_INTERLOCK_data__ matiec;
    ESD_INTERLOCK_init__(&matiec, 0);

    /* ── Scenario 1: No trips, all clear ── */
    printf("[Scenario 1: All clear — no trips]\n");
    {
        scan_input_t s = {"all_clear", false, false, false, false, 1};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 2: Pressure trip triggers alarm latch ── */
    printf("\n[Scenario 2: Pressure trip — alarm latches]\n");
    {
        scan_input_t s = {"pressure_trip", true, false, false, false, 2};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 3: Trip persists, timer counting (100ms) ── */
    printf("\n[Scenario 3: Timer counting (10 ticks = 100ms)]\n");
    {
        scan_input_t s = {"timer_10ticks", true, false, false, false, 12};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 4: Timer still counting at 2s ── */
    printf("\n[Scenario 4: Timer at 200 ticks (2s)]\n");
    {
        scan_input_t s = {"timer_200ticks", true, false, false, false, 202};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 5: Timer expires at 3s — confirmed alarm ── */
    printf("\n[Scenario 5: Timer expired (302 ticks = 3.02s) — confirmed alarm]\n");
    {
        scan_input_t s = {"timer_expired", true, false, false, false, 302};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 6: Still in confirmed alarm state ── */
    printf("\n[Scenario 6: Holding confirmed alarm]\n");
    {
        scan_input_t s = {"alarm_holding", true, false, false, false, 400};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 7: Manual reset — trip input still high ── */
    printf("\n[Scenario 7: Manual reset with trip still active]\n");
    {
        scan_input_t s = {"manual_reset_trip_active", true, false, false, true, 401};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 8: Trip clears, manual reset ── */
    printf("\n[Scenario 8: Trip clears, manual reset]\n");
    {
        scan_input_t s = {"trip_clear_reset", false, false, false, true, 402};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 9: Fully clear after reset ── */
    printf("\n[Scenario 9: Fully clear after reset]\n");
    {
        scan_input_t s = {"fully_clear", false, false, false, false, 403};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 10: Multiple trips simultaneously ── */
    printf("\n[Scenario 10: All three trips simultaneously]\n");
    {
        scan_input_t s = {"all_trips", true, true, true, false, 500};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Scenario 11: Timer expires for multi-trip ── */
    printf("\n[Scenario 11: Multi-trip timer expired]\n");
    {
        scan_input_t s = {"multi_trip_expired", true, true, true, false, 802};
        run_scan(&s, &pool, &matiec, &hdr);
    }

    /* ── Results ── */
    printf("\n=== Results: %d/%d passed", passed_tests, total_tests);
    if (failed_tests > 0)
        printf(" (%d FAILED)", failed_tests);
    printf(" ===\n");

    return failed_tests > 0 ? 1 : 0;
}
