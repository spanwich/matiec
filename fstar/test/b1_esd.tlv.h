/**
 * b1_esd.tlv.h — Hand-encoded TLV binary for B1 (ESD Interlock).
 *
 * Source: benchmarks/b1_esd.st
 * Encoding: D5 format (ELSIFs flattened into nested IF_BLOCKs)
 *
 * Variable Pool Mapping:
 *   BOOL pool (n_bools = 12):
 *     0: pressure_high  (input)
 *     1: temp_high      (input)
 *     2: flow_low       (input)
 *     3: manual_reset   (input)
 *     4: any_trip        (internal)
 *     5: latched         (internal)
 *     6: shutdown_cmd    (output)
 *     7: alarm_out       (output)
 *     8: [scratch] alarm_latch.Q1 — SR output
 *     9: [scratch] alarm_delay.Q  — TON output
 *    10: [scratch] expr temp 0
 *    11: [scratch] expr temp 1
 *
 *   INT pool (n_ints = 0)
 *
 *   FB instances:
 *     n_sr  = 1:  [0] alarm_latch
 *     n_ton = 1:  [0] alarm_delay
 *     (all other FB counts = 0)
 *
 * Total: 20 (header) + 0 (init) + 95 (body) = 115 bytes.
 */

#ifndef B1_ESD_TLV_H
#define B1_ESD_TLV_H

#include <stdint.h>

static const uint8_t b1_esd_tlv[] = {
    /* === HEADER (20 bytes) === */
    0x4C, 0x50,       /* magic "LP"              */
    0x01,             /* version 1               */
    0x00,             /* flags (reserved)        */
    0x0C,             /* n_bools = 12            */
    0x00,             /* n_ints = 0              */
    0x01,             /* n_ton = 1               */
    0x00,             /* n_tof = 0               */
    0x00,             /* n_tp = 0                */
    0x00,             /* n_ctu = 0               */
    0x00,             /* n_ctd = 0               */
    0x00,             /* n_ctud = 0              */
    0x01,             /* n_sr = 1                */
    0x00,             /* n_rs = 0                */
    0x00,             /* n_rtrig = 0             */
    0x00,             /* n_ftrig = 0             */
    0x00, 0x00,       /* init_len = 0            */
    0x5F, 0x00,       /* body_len = 95           */

    /* === BODY (95 bytes) === */

    /* any_trip := pressure_high OR temp_high OR flow_low */
    /*   scratch[10] := bools[0] OR bools[1] */
    0x12, 0x03, 0x00, 0x0A, 0x00, 0x01,   /* BOOL_OR dst=10 a=0 b=1 */
    /*   bools[4] := scratch[10] OR bools[2] */
    0x12, 0x03, 0x00, 0x04, 0x0A, 0x02,   /* BOOL_OR dst=4  a=10 b=2 */

    /* alarm_latch(S1 := any_trip, R := manual_reset) */
    0x70, 0x04, 0x00, 0x00, 0x04, 0x03, 0x08,  /* SR_CALL idx=0 s1=4 r=3 q1=8 */

    /* latched := alarm_latch.Q1 */
    0x14, 0x02, 0x00, 0x05, 0x08,         /* COPY_BOOL dst=5 src=8 */

    /* alarm_delay(IN := latched, PT := T#3s = 300 ticks @ 10ms) */
    0x50, 0x07, 0x00,                     /* TON_CALL tag + len=7 */
    0x00,                                 /* idx=0 */
    0x05,                                 /* in_b=5 (latched) */
    0x2C, 0x01, 0x00, 0x00,              /* pt=300 LE */
    0x09,                                 /* q_b=9 */

    /* Pre-compute outer IF condition: scratch[10] := latched AND alarm_delay.Q */
    0x11, 0x03, 0x00, 0x0A, 0x05, 0x09,   /* BOOL_AND dst=10 a=5 b=9 */

    /* === Outer IF_BLOCK (D5 format: ELSIF flattened) ===
       IF latched AND alarm_delay.Q THEN
         shutdown_cmd := TRUE; alarm_out := TRUE
       ELSE
         (nested IF for former ELSIF)
       END_IF
    */
    0x40,                                  /* tag = IF_BLOCK     */
    0x34, 0x00,                            /* len = 52 (payload) */
    0x0A,                                  /* cond = 10          */
    0x02,                                  /* then_count = 2     */
    0x01,                                  /* else_count = 1     */
    0x08, 0x00,                            /* then_len = 8       */
    0x25, 0x00,                            /* else_len = 37      */
    /* THEN body (8 bytes): shutdown_cmd := TRUE; alarm_out := TRUE */
    0x01, 0x01, 0x00, 0x06,               /*   LOAD_TRUE dst=6  */
    0x01, 0x01, 0x00, 0x07,               /*   LOAD_TRUE dst=7  */
    /* ELSE body (37 bytes): nested IF for former ELSIF */
    /*   Pre-compute ELSIF cond: scratch[11] := NOT alarm_delay.Q */
    0x10, 0x02, 0x00, 0x0B, 0x09,         /*   BOOL_NOT dst=11 src=9 */
    /*   scratch[11] := latched AND NOT alarm_delay.Q */
    0x11, 0x03, 0x00, 0x0B, 0x05, 0x0B,   /*   BOOL_AND dst=11 a=5 b=11 */
    /*   Inner IF_BLOCK (former ELSIF) */
    0x40,                                  /*   tag = IF_BLOCK   */
    0x17, 0x00,                            /*   len = 23         */
    0x0B,                                  /*   cond = 11        */
    0x02,                                  /*   then_count = 2   */
    0x02,                                  /*   else_count = 2   */
    0x08, 0x00,                            /*   then_len = 8     */
    0x08, 0x00,                            /*   else_len = 8     */
    /* THEN body (8 bytes): shutdown_cmd := FALSE; alarm_out := FALSE */
    0x02, 0x01, 0x00, 0x06,               /*   LOAD_FALSE dst=6 */
    0x02, 0x01, 0x00, 0x07,               /*   LOAD_FALSE dst=7 */
    /* ELSE body (8 bytes): shutdown_cmd := FALSE; alarm_out := FALSE */
    0x02, 0x01, 0x00, 0x06,               /*   LOAD_FALSE dst=6 */
    0x02, 0x01, 0x00, 0x07,               /*   LOAD_FALSE dst=7 */
};

/* Verify total size at compile time */
_Static_assert(sizeof(b1_esd_tlv) == 115,
    "B1 TLV binary must be exactly 115 bytes");

/* Pool index constants for test access */
#define B1_PRESSURE_HIGH  0
#define B1_TEMP_HIGH      1
#define B1_FLOW_LOW       2
#define B1_MANUAL_RESET   3
#define B1_ANY_TRIP       4
#define B1_LATCHED        5
#define B1_SHUTDOWN_CMD   6
#define B1_ALARM_OUT      7

#endif /* B1_ESD_TLV_H */
