/**
 * lowplc_interp.h -- Thin adapter wrapping the KaRaMeL-extracted verified
 * interpreter (LowPLC_Interp_Low.c) behind the same API used by the E2E
 * test harness.
 *
 * Every call to exec_init / exec_scan is dispatched through
 * LowPLC_Interp_Low_exec_stmts_low -- the F*-verified, KaRaMeL-extracted
 * interpreter.  No hand-written dispatch code exists in this file.
 *
 * Source: fstar/build_low/LowPLC_Interp_Low.{c,h}
 *         (extracted from fstar/src/LowPLC.Interp.Low.fst)
 */
#ifndef LOWPLC_INTERP_H
#define LOWPLC_INTERP_H

#ifdef LOWPLC_FIRMWARE_BUILD
#error "Do not include lowplc_interp.h in firmware — use LowPLC_Interp_Low.h directly"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* KaRaMeL-extracted interpreter */
#include "LowPLC_Interp_Low.h"

/* ── TLV Opcode Tags (aliases for extracted extern constants) ────── */

#define TAG_LOAD_TRUE   0x01
#define TAG_LOAD_FALSE  0x02
#define TAG_LOAD_INT    0x03
#define TAG_BOOL_NOT    0x10
#define TAG_BOOL_AND    0x11
#define TAG_BOOL_OR     0x12
#define TAG_BOOL_XOR    0x13
#define TAG_COPY_BOOL   0x14
#define TAG_INT_ADD     0x20
#define TAG_INT_SUB     0x21
#define TAG_INT_MUL     0x22
#define TAG_COPY_INT    0x23
#define TAG_CMP_EQ      0x30
#define TAG_CMP_NE      0x31
#define TAG_CMP_LT      0x32
#define TAG_CMP_GT      0x33
#define TAG_CMP_LE      0x34
#define TAG_CMP_GE      0x35
#define TAG_IF_BLOCK    0x40
#define TAG_FOR_BLOCK   0x41
#define TAG_TON_CALL    0x50
#define TAG_TOF_CALL    0x51
#define TAG_TP_CALL     0x52
#define TAG_CTU_CALL    0x60
#define TAG_CTD_CALL    0x61
#define TAG_CTUD_CALL   0x62
#define TAG_SR_CALL     0x70
#define TAG_RS_CALL     0x71
#define TAG_RTRIG_CALL  0x72
#define TAG_FTRIG_CALL  0x73

/* ── Pool Limits (matching tlv_format.md S3) ────────────────────── */

#define MAX_BOOLS   64
#define MAX_INTS    32
#define MAX_FB_INST  8

/* ── Variable Pool ──────────────────────────────────────────────── */

/** Variable pool with inline arrays -- test-side storage.
    The adapter builds a low_pool pointing into these arrays. */
typedef struct {
    /* Scalar pools */
    bool     bools[MAX_BOOLS];
    int16_t  ints[MAX_INTS];
    /* Pool sizes (from header) */
    uint8_t  n_bools;
    uint8_t  n_ints;
    /* FB state pools */
    LowPLC_Interp_Low_ton_flat   tons[MAX_FB_INST];
    LowPLC_Interp_Low_tof_flat   tofs[MAX_FB_INST];
    LowPLC_Interp_Low_tp_flat    tps[MAX_FB_INST];
    LowPLC_Interp_Low_ctu_flat   ctus[MAX_FB_INST];
    LowPLC_Interp_Low_ctd_flat   ctds[MAX_FB_INST];
    LowPLC_Interp_Low_ctud_flat  ctuds[MAX_FB_INST];
    uint8_t  srs[MAX_FB_INST];
    uint8_t  rss[MAX_FB_INST];
    uint8_t  rtrigs[MAX_FB_INST];
    uint8_t  ftrigs[MAX_FB_INST];
    /* FB instance counts */
    uint8_t  n_ton, n_tof, n_tp;
    uint8_t  n_ctu, n_ctd, n_ctud;
    uint8_t  n_sr, n_rs, n_rtrig, n_ftrig;
    /* Current tick */
    uint32_t tick;
    /* ADR-009: scan period for pt_us to pt_ticks conversion */
    uint32_t scan_cycle_period_us;
} var_pool_t;

/* ── TLV Header ─────────────────────────────────────────────────── */

typedef struct {
    uint8_t  n_bools, n_ints;
    uint8_t  n_ton, n_tof, n_tp;
    uint8_t  n_ctu, n_ctd, n_ctud;
    uint8_t  n_sr, n_rs, n_rtrig, n_ftrig;
    uint16_t init_len, body_len;
} tlv_header_t;

/* ── Byte Parsing Helpers ───────────────────────────────────────── */

static inline uint16_t read_u16_le(const uint8_t *s, size_t off, size_t len) {
    if (off + 2 > len) return 0;
    return (uint16_t)s[off] | ((uint16_t)s[off+1] << 8);
}

/* ── Header Parse ───────────────────────────────────────────────── */

static inline int parse_header(const uint8_t *code, size_t code_len,
                               tlv_header_t *h) {
    if (code_len < 20) return -1;
    if (code[0] != 0x4C || code[1] != 0x50) return -1;
    if (code[2] != 0x01) return -1;

    h->n_bools  = code[4];
    h->n_ints   = code[5];
    h->n_ton    = code[6];
    h->n_tof    = code[7];
    h->n_tp     = code[8];
    h->n_ctu    = code[9];
    h->n_ctd    = code[10];
    h->n_ctud   = code[11];
    h->n_sr     = code[12];
    h->n_rs     = code[13];
    h->n_rtrig  = code[14];
    h->n_ftrig  = code[15];
    h->init_len = read_u16_le(code, 16, 20);
    h->body_len = read_u16_le(code, 18, 20);

    if (h->n_bools > MAX_BOOLS || h->n_ints > MAX_INTS) return -2;
    if (h->n_ton > MAX_FB_INST || h->n_tof > MAX_FB_INST) return -2;
    if (h->n_tp > MAX_FB_INST || h->n_ctu > MAX_FB_INST) return -2;
    if (h->n_ctd > MAX_FB_INST || h->n_ctud > MAX_FB_INST) return -2;
    if (h->n_sr > MAX_FB_INST || h->n_rs > MAX_FB_INST) return -2;
    if (h->n_rtrig > MAX_FB_INST || h->n_ftrig > MAX_FB_INST) return -2;
    if (20 + h->init_len + h->body_len > code_len) return -3;

    return 0;
}

/* ── Pool Initialization ────────────────────────────────────────── */

static inline void init_pool(var_pool_t *p, const tlv_header_t *h) {
    memset(p, 0, sizeof(*p));
    p->n_bools = h->n_bools;
    p->n_ints  = h->n_ints;
    p->n_ton   = h->n_ton;
    p->n_tof   = h->n_tof;
    p->n_tp    = h->n_tp;
    p->n_ctu   = h->n_ctu;
    p->n_ctd   = h->n_ctd;
    p->n_ctud  = h->n_ctud;
    p->n_sr    = h->n_sr;
    p->n_rs    = h->n_rs;
    p->n_rtrig = h->n_rtrig;
    p->n_ftrig = h->n_ftrig;
}

/* ── Adapter: build low_pool pointing into var_pool_t ───────────── */

static inline LowPLC_Interp_Low_low_pool
make_low_pool(var_pool_t *p) {
    LowPLC_Interp_Low_low_pool lp;
    lp.lp_bools  = p->bools;
    lp.lp_ints   = p->ints;
    lp.lp_tons   = p->tons;
    lp.lp_tofs   = p->tofs;
    lp.lp_tps    = p->tps;
    lp.lp_ctus   = p->ctus;
    lp.lp_ctds   = p->ctds;
    lp.lp_ctuds  = p->ctuds;
    lp.lp_srs    = p->srs;
    lp.lp_rss    = p->rss;
    lp.lp_rtrigs = p->rtrigs;
    lp.lp_ftrigs = p->ftrigs;
    lp.lp_n_bools = p->n_bools;
    lp.lp_n_ints  = p->n_ints;
    lp.lp_n_ton   = p->n_ton;
    lp.lp_n_tof   = p->n_tof;
    lp.lp_n_tp    = p->n_tp;
    lp.lp_n_ctu   = p->n_ctu;
    lp.lp_n_ctd   = p->n_ctd;
    lp.lp_n_ctud  = p->n_ctud;
    lp.lp_n_sr    = p->n_sr;
    lp.lp_n_rs    = p->n_rs;
    lp.lp_n_rtrig = p->n_rtrig;
    lp.lp_n_ftrig = p->n_ftrig;
    lp.lp_tick    = p->tick;
    lp.lp_scan_period_us = p->scan_cycle_period_us;
    return lp;
}

/* ── Entry Points (dispatch to extracted interpreter) ───────────── */

/** Execute the INIT section (called once). */
static inline void exec_init(const uint8_t *code, size_t code_len,
                             const tlv_header_t *h, var_pool_t *p, int gas) {
    LowPLC_Interp_Low_low_pool lp = make_low_pool(p);
    LowPLC_Interp_Low_exec_stmts_low(
        (uint8_t *)code, (uint32_t)code_len,
        20, h->init_len, lp, (uint32_t)gas);
}

/** Execute the BODY section (called each scan cycle). */
static inline void exec_scan(const uint8_t *code, size_t code_len,
                             const tlv_header_t *h, var_pool_t *p, int gas) {
    LowPLC_Interp_Low_low_pool lp = make_low_pool(p);
    LowPLC_Interp_Low_exec_stmts_low(
        (uint8_t *)code, (uint32_t)code_len,
        20 + h->init_len, h->body_len, lp, (uint32_t)gas);
}

#endif /* LOWPLC_INTERP_H */
