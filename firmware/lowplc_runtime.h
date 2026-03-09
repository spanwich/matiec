/**
 * lowplc_runtime.h — Verified-safe LowPLC baremetal runtime.
 *
 * Combines four layers into a single load-time validation + execution API:
 *   1. Header validation (magic, version, pool bounds)
 *   2. Opcode stream validation (known tags, index bounds, nesting depth)
 *   3. WCET computation (compositional, mirrors LowPLC.WCET.fst)
 *   4. Interpreter execution (mirrors LowPLC.Interp.Spec.fst)
 *
 * Usage:
 *   uint32_t wcet;
 *   if (!lowplc_load_program(tlv, len, scan_budget_cycles, &wcet)) {
 *       // TLV rejected — validation or WCET budget exceeded
 *       return;
 *   }
 *   // Program accepted — wcet contains worst-case bound in cycles
 *   while (1) {
 *       pool.tick = current_tick;
 *       lowplc_exec_scan(&pool);
 *       wait_for_scan_period();
 *   }
 *
 * Source: fstar/src/LowPLC.WCET.fst, fstar/src/LowPLC.Interp.Spec.fst
 */
#ifndef LOWPLC_RUNTIME_H
#define LOWPLC_RUNTIME_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ── Constants ────────────────────────────────────────────────────── */

#define LOWPLC_MAX_BOOLS       64
#define LOWPLC_MAX_INTS        32
#define LOWPLC_MAX_TON          8
#define LOWPLC_MAX_TOF          8
#define LOWPLC_MAX_TP           8
#define LOWPLC_MAX_CTU          8
#define LOWPLC_MAX_CTD          8
#define LOWPLC_MAX_CTUD         4
#define LOWPLC_MAX_SR           8
#define LOWPLC_MAX_RS           8
#define LOWPLC_MAX_RTRIG        8
#define LOWPLC_MAX_FTRIG        8
#define LOWPLC_MAX_NESTING      5
#define LOWPLC_MAX_FOR_ITERS  255
#define LOWPLC_INTERP_GAS   65536

/* ── Error Codes ──────────────────────────────────────────────────── */

typedef enum {
    LOWPLC_OK              =  0,
    LOWPLC_ERR_HEADER      = -1,  /* Bad magic, version, or truncated */
    LOWPLC_ERR_POOL_BOUNDS = -2,  /* Pool count exceeds MAX */
    LOWPLC_ERR_LENGTH      = -3,  /* init_len + body_len exceeds TLV size */
    LOWPLC_ERR_OPCODES     = -4,  /* Unknown tag, bad length, OOB index */
    LOWPLC_ERR_WCET        = -5,  /* WCET exceeds scan budget */
} lowplc_err_t;

/* ── Public API ───────────────────────────────────────────────────── */

/**
 * Validate a TLV program and check WCET budget.
 *
 * Performs all four validation layers. On success, initialises the
 * internal state (pool + header cache) and runs exec_init.
 *
 * @param tlv                 TLV program binary
 * @param len                 Length in bytes
 * @param scan_period_cycles  Scan cycle budget (0 = skip WCET check)
 * @param wcet_out            [out] Computed WCET in cycles (may be NULL)
 * @return LOWPLC_OK on success, negative error code on failure
 */
lowplc_err_t lowplc_load_program(const uint8_t *tlv, size_t len,
                                 uint32_t scan_period_cycles,
                                 uint32_t *wcet_out);

/**
 * Execute one scan cycle (BODY section).
 *
 * Must be called after a successful lowplc_load_program().
 * Caller should set the tick counter before each call:
 *     lowplc_set_tick(current_tick);
 *     lowplc_exec_scan();
 */
void lowplc_exec_scan(void);

/** Set the current tick counter for timer FBs. */
void lowplc_set_tick(uint32_t tick);

/** Set a bool input in the pool. */
void lowplc_set_bool(unsigned idx, bool val);

/** Read a bool output from the pool. */
bool lowplc_get_bool(unsigned idx);

/** Read an int output from the pool. */
int16_t lowplc_get_int(unsigned idx);

#endif /* LOWPLC_RUNTIME_H */
