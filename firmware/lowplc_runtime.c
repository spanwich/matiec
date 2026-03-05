/**
 * lowplc_runtime.c — Verified-safe LowPLC baremetal runtime implementation.
 *
 * Layer 1+2: EverParse-generated TLV validator (everparse/LowPLC_TLV.3d)
 *            Validates header, pool bounds, all opcode tags, index bounds,
 *            nesting depth (<=5), and byte-size consistency in a single call.
 * Layer 3:   WCET computation (transliterated from LowPLC.WCET.fst)
 * Layer 4:   Interpreter (via lowplc_interp.h)
 *
 * Source: everparse/LowPLC_TLV.3d, fstar/src/LowPLC.WCET.fst,
 *         fstar/test/lowplc_interp.h
 */

#include "lowplc_runtime.h"

/* ── Bare-metal EverParse overrides ──────────────────────────────── */
/* Suppress stdio/stdlib dependencies from EverParse runtime headers.
 * Must be defined before including any EverParse header. */
#ifndef KRML_HOST_PRINTF
#define KRML_HOST_PRINTF(...) ((void)0)
#endif
#ifndef KRML_HOST_EXIT
#define KRML_HOST_EXIT(x) while(1) {}
#endif

/* EverParse-generated TLV validator (Layer 1+2) */
#include "LowPLC_TLVWrapper.h"

/* Pull in the interpreter (header-only, unity build) */
#include "lowplc_interp.h"

/* ── EverParse error callback (bare-metal: no-op) ────────────────── */
void LowPLC_TLVEverParseError(const char *StructName,
                               const char *FieldName,
                               const char *Reason) {
    (void)StructName;
    (void)FieldName;
    (void)Reason;
}

/* ── Internal State ───────────────────────────────────────────────── */

static const uint8_t *g_tlv;
static size_t         g_tlv_len;
static tlv_header_t   g_header;
static var_pool_t     g_pool;

/* ── Per-opcode cost constants (placeholder 0 — from DWT Phase 3) ─ */

static const uint32_t cost_dispatch     = 0;
static const uint32_t cost_if_overhead  = 0;
static const uint32_t cost_for_overhead = 0;

static const uint32_t cost_load_true  = 0, cost_load_false = 0, cost_load_int   = 0;
static const uint32_t cost_bool_not   = 0, cost_bool_and   = 0, cost_bool_or    = 0;
static const uint32_t cost_bool_xor   = 0, cost_copy_bool  = 0;
static const uint32_t cost_int_add    = 0, cost_int_sub    = 0, cost_int_mul    = 0;
static const uint32_t cost_copy_int   = 0;
static const uint32_t cost_cmp_eq     = 0, cost_cmp_ne     = 0, cost_cmp_lt     = 0;
static const uint32_t cost_cmp_gt     = 0, cost_cmp_le     = 0, cost_cmp_ge     = 0;
static const uint32_t cost_ton_call   = 0, cost_tof_call   = 0, cost_tp_call    = 0;
static const uint32_t cost_ctu_call   = 0, cost_ctd_call   = 0, cost_ctud_call  = 0;
static const uint32_t cost_sr_call    = 0, cost_rs_call    = 0;
static const uint32_t cost_rtrig_call = 0, cost_ftrig_call = 0;

/* ── Header Field Extraction ─────────────────────────────────────── */
/* No validation here — EverParse (Layer 1+2) already verified all
 * header fields, pool bounds, and structural integrity. This just
 * reads the validated fields into the interpreter's header struct. */

static void extract_header(const uint8_t *tlv, tlv_header_t *h) {
    h->n_bools = tlv[4];
    h->n_ints  = tlv[5];
    h->n_ton   = tlv[6];
    h->n_tof   = tlv[7];
    h->n_tp    = tlv[8];
    h->n_ctu   = tlv[9];
    h->n_ctd   = tlv[10];
    h->n_ctud  = tlv[11];
    h->n_sr    = tlv[12];
    h->n_rs    = tlv[13];
    h->n_rtrig = tlv[14];
    h->n_ftrig = tlv[15];
    h->init_len = read_u16_le(tlv, 16, 20);
    h->body_len = read_u16_le(tlv, 18, 20);
}

/* ── Layer 3: WCET Computation ───────────────────────────────────── */

static uint32_t atomic_cost(uint8_t tag) {
    switch (tag) {
    case TAG_LOAD_TRUE:  return cost_load_true;
    case TAG_LOAD_FALSE: return cost_load_false;
    case TAG_LOAD_INT:   return cost_load_int;
    case TAG_BOOL_NOT:   return cost_bool_not;
    case TAG_BOOL_AND:   return cost_bool_and;
    case TAG_BOOL_OR:    return cost_bool_or;
    case TAG_BOOL_XOR:   return cost_bool_xor;
    case TAG_COPY_BOOL:  return cost_copy_bool;
    case TAG_INT_ADD:    return cost_int_add;
    case TAG_INT_SUB:    return cost_int_sub;
    case TAG_INT_MUL:    return cost_int_mul;
    case TAG_COPY_INT:   return cost_copy_int;
    case TAG_CMP_EQ:     return cost_cmp_eq;
    case TAG_CMP_NE:     return cost_cmp_ne;
    case TAG_CMP_LT:     return cost_cmp_lt;
    case TAG_CMP_GT:     return cost_cmp_gt;
    case TAG_CMP_LE:     return cost_cmp_le;
    case TAG_CMP_GE:     return cost_cmp_ge;
    case TAG_TON_CALL:   return cost_ton_call;
    case TAG_TOF_CALL:   return cost_tof_call;
    case TAG_TP_CALL:    return cost_tp_call;
    case TAG_CTU_CALL:   return cost_ctu_call;
    case TAG_CTD_CALL:   return cost_ctd_call;
    case TAG_CTUD_CALL:  return cost_ctud_call;
    case TAG_SR_CALL:    return cost_sr_call;
    case TAG_RS_CALL:    return cost_rs_call;
    case TAG_RTRIG_CALL: return cost_rtrig_call;
    case TAG_FTRIG_CALL: return cost_ftrig_call;
    default: return 0;
    }
}

static uint32_t wcet_stmts(const uint8_t *code, size_t code_len,
                            size_t off, size_t rem, int gas);

static uint32_t wcet_if_block(const uint8_t *code, size_t code_len,
                               size_t poff, size_t plen, int gas) {
    if (gas <= 0 || poff + 7 > code_len) return 0;

    size_t then_len = read_u16_le(code, poff + 3, code_len);
    size_t else_len = read_u16_le(code, poff + 5, code_len);
    size_t then_off = poff + 7;
    size_t else_off = then_off + then_len;

    if (then_off + then_len > code_len) return 0;
    if (else_off + else_len > code_len) return 0;

    uint32_t tc = wcet_stmts(code, code_len, then_off, then_len, gas - 1);
    uint32_t ec = wcet_stmts(code, code_len, else_off, else_len, gas - 1);
    return cost_if_overhead + (tc > ec ? tc : ec);
}

static uint32_t wcet_for_block(const uint8_t *code, size_t code_len,
                                size_t poff, size_t plen, int gas) {
    if (gas <= 0 || poff + 8 > code_len) return 0;

    int16_t start_v = read_i16_le(code, poff + 1, code_len);
    int16_t end_v   = read_i16_le(code, poff + 3, code_len);
    size_t body_len = read_u16_le(code, poff + 6, code_len);
    size_t body_off = poff + 8;

    if (body_off + body_len > code_len) return 0;

    int iters = (end_v >= start_v) ? (int)end_v - (int)start_v + 1 : 0;
    uint32_t bc = wcet_stmts(code, code_len, body_off, body_len, gas - 1);
    return cost_for_overhead + (uint32_t)iters * (bc + cost_for_overhead);
}

static uint32_t wcet_stmts(const uint8_t *code, size_t code_len,
                            size_t off, size_t rem, int gas) {
    uint32_t total = 0;

    while (gas > 0 && rem >= 3 && off + 3 <= code_len) {
        uint8_t tag = code[off];
        size_t len = read_u16_le(code, off + 1, code_len);
        if (3 + len > rem || off + 3 + len > code_len) break;

        uint32_t sc;
        if (tag == TAG_IF_BLOCK)
            sc = wcet_if_block(code, code_len, off + 3, len, gas - 1);
        else if (tag == TAG_FOR_BLOCK)
            sc = wcet_for_block(code, code_len, off + 3, len, gas - 1);
        else
            sc = atomic_cost(tag) + cost_dispatch;

        total += sc;
        off += 3 + len;
        rem -= 3 + len;
        gas--;
    }
    return total;
}

/* ── Public API Implementation ───────────────────────────────────── */

lowplc_err_t lowplc_load_program(const uint8_t *tlv, size_t len,
                                 uint32_t scan_period_cycles,
                                 uint32_t *wcet_out) {
    /* Layer 1+2: EverParse TLV validation (header + pool bounds +
     * opcode tags + index bounds + nesting depth + byte-size checks).
     * Single call replaces all hand-written validators. */
    if (len < 20 || len > UINT32_MAX)
        return LOWPLC_ERR_HEADER;
    if (!LowPlcTlvCheckLowPlcprogram((uint8_t *)(uintptr_t)tlv, (uint32_t)len))
        return LOWPLC_ERR_OPCODES;

    /* Extract validated header fields for interpreter + WCET */
    extract_header(tlv, &g_header);

    /* Layer 3: WCET computation (BODY only — runs every scan) */
    size_t body_off = 20 + g_header.init_len;
    uint32_t wcet = wcet_stmts(tlv, len, body_off, g_header.body_len,
                               LOWPLC_INTERP_GAS);

    if (wcet_out) *wcet_out = wcet;

    if (scan_period_cycles > 0 && wcet > scan_period_cycles)
        return LOWPLC_ERR_WCET;

    /* Layer 4: Init pool + exec_init */
    g_tlv     = tlv;
    g_tlv_len = len;
    init_pool(&g_pool, &g_header);
    exec_init(tlv, len, &g_header, &g_pool, LOWPLC_INTERP_GAS);

    return LOWPLC_OK;
}

void lowplc_exec_scan(void) {
    exec_scan(g_tlv, g_tlv_len, &g_header, &g_pool, LOWPLC_INTERP_GAS);
}

void lowplc_set_tick(uint32_t tick) {
    g_pool.tick = tick;
}

bool lowplc_get_bool(unsigned idx) {
    return get_bool(&g_pool, idx);
}

int16_t lowplc_get_int(unsigned idx) {
    return get_int(&g_pool, idx);
}
