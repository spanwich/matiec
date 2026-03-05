/**
 * lowplc_interp.h — C TLV interpreter mirroring the verified F* spec.
 *
 * This is a direct transliteration of LowPLC.Interp.Spec into C.
 * Each function corresponds 1:1 to its F* counterpart.  The variable
 * pool uses flat arrays matching the Seq-based var_pool type.
 *
 * The interpreter uses the verified D2 combinator C extractions
 * (ton_step_c, sr_step_c, etc.) for FB dispatch.
 *
 * Source: fstar/src/LowPLC.Interp.Spec.fst
 */
#ifndef LOWPLC_INTERP_H
#define LOWPLC_INTERP_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ── Pool Limits (matching tlv_format.md §3) ────────────────────── */

#define MAX_BOOLS   64
#define MAX_INTS    32
#define MAX_FB_INST  8

/* ── TLV Opcode Tags (matching LowPLC.Interp.Spec §6) ──────────── */

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

/* ── Variable Pool ──────────────────────────────────────────────── */

/** TON timer state — matches LowPLC.TON.ton_state */
typedef struct {
    uint8_t  phase;       /* 0=IDLE, 1=TIMING, 2=DONE */
    uint32_t start_tick;
} ton_state_t;

/** TOF timer state — matches LowPLC.TOF.tof_state */
typedef struct {
    uint8_t  phase;
    uint8_t  prev_in;
    uint32_t start_tick;
} tof_state_t;

/** TP timer state — matches LowPLC.TP.tp_state */
typedef struct {
    uint8_t  phase;
    uint8_t  prev_in;
    uint32_t start_tick;
} tp_state_t;

/** CTU counter state — matches LowPLC.CTU.ctu_state */
typedef struct {
    int16_t cv;
    uint8_t prev_cu;
} ctu_state_t;

/** CTD counter state — matches LowPLC.CTD.ctd_state */
typedef struct {
    int16_t cv;
    uint8_t prev_cd;
} ctd_state_t;

/** CTUD counter state — matches LowPLC.CTUD.ctud_state */
typedef struct {
    int16_t cv;
    uint8_t prev_cu;
    uint8_t prev_cd;
} ctud_state_t;

/** SR state — matches LowPLC.SR.sr_state */
typedef struct { uint8_t q; } sr_state_t;

/** RS state — matches LowPLC.RS.rs_state */
typedef struct { uint8_t q; } rs_state_t;

/** R_TRIG state — matches LowPLC.R_TRIG.r_trig_state */
typedef struct { uint8_t m; } rtrig_state_t;

/** F_TRIG state — matches LowPLC.F_TRIG.f_trig_state */
typedef struct { uint8_t m; } ftrig_state_t;

/** Variable pool — C equivalent of LowPLC.Interp.State.var_pool */
typedef struct {
    /* Scalar pools */
    bool     bools[MAX_BOOLS];
    int16_t  ints[MAX_INTS];
    /* Pool sizes (from header) */
    uint8_t  n_bools;
    uint8_t  n_ints;
    /* FB state pools */
    ton_state_t   tons[MAX_FB_INST];
    tof_state_t   tofs[MAX_FB_INST];
    tp_state_t    tps[MAX_FB_INST];
    ctu_state_t   ctus[MAX_FB_INST];
    ctd_state_t   ctds[MAX_FB_INST];
    ctud_state_t  ctuds[MAX_FB_INST];
    sr_state_t    srs[MAX_FB_INST];
    rs_state_t    rss[MAX_FB_INST];
    rtrig_state_t rtrigs[MAX_FB_INST];
    ftrig_state_t ftrigs[MAX_FB_INST];
    /* FB instance counts */
    uint8_t  n_ton, n_tof, n_tp;
    uint8_t  n_ctu, n_ctd, n_ctud;
    uint8_t  n_sr, n_rs, n_rtrig, n_ftrig;
    /* Current tick */
    uint32_t tick;
} var_pool_t;

/* ── TLV Header ─────────────────────────────────────────────────── */

typedef struct {
    uint8_t  n_bools, n_ints;
    uint8_t  n_ton, n_tof, n_tp;
    uint8_t  n_ctu, n_ctd, n_ctud;
    uint8_t  n_sr, n_rs, n_rtrig, n_ftrig;
    uint16_t init_len, body_len;
} tlv_header_t;

/* ── Byte Parsing Helpers (matching State.fst) ──────────────────── */

static inline uint16_t read_u16_le(const uint8_t *s, size_t off, size_t len) {
    if (off + 2 > len) return 0;
    return (uint16_t)s[off] | ((uint16_t)s[off+1] << 8);
}

static inline uint32_t read_u32_le(const uint8_t *s, size_t off, size_t len) {
    if (off + 4 > len) return 0;
    return (uint32_t)s[off]
         | ((uint32_t)s[off+1] << 8)
         | ((uint32_t)s[off+2] << 16)
         | ((uint32_t)s[off+3] << 24);
}

static inline int16_t read_i16_le(const uint8_t *s, size_t off, size_t len) {
    if (off + 2 > len) return 0;
    uint16_t raw = (uint16_t)s[off] | ((uint16_t)s[off+1] << 8);
    return (int16_t)raw;
}

/* ── Wrapping INT Arithmetic (DDR-003) ──────────────────────────── */

static inline int16_t wrap_i16(int32_t x) {
    return (int16_t)(((x + 32768) % 65536) - 32768);
}

static inline int16_t add_wrap(int16_t a, int16_t b) {
    return wrap_i16((int32_t)a + (int32_t)b);
}

static inline int16_t sub_wrap(int16_t a, int16_t b) {
    return wrap_i16((int32_t)a - (int32_t)b);
}

static inline int16_t mul_wrap(int16_t a, int16_t b) {
    return wrap_i16((int32_t)a * (int32_t)b);
}

/* ── Safe Pool Accessors (matching State.fst) ───────────────────── */

static inline bool get_bool(const var_pool_t *p, unsigned idx) {
    return idx < p->n_bools ? p->bools[idx] : false;
}

static inline void set_bool(var_pool_t *p, unsigned idx, bool v) {
    if (idx < p->n_bools) p->bools[idx] = v;
}

static inline int16_t get_int(const var_pool_t *p, unsigned idx) {
    return idx < p->n_ints ? p->ints[idx] : 0;
}

static inline void set_int(var_pool_t *p, unsigned idx, int16_t v) {
    if (idx < p->n_ints) p->ints[idx] = v;
}

/* ── FB Step Functions (mirroring verified combinators) ──────────── */

/** TON step — mirrors LowPLC.TON.ton_step */
static inline void ton_step(ton_state_t *s, bool in_val, uint32_t pt,
                            uint32_t tick, bool *q_out, uint32_t *et_out) {
    if (s->phase == 0) { /* IDLE */
        if (in_val) {
            s->phase = 1;
            s->start_tick = tick;
            *q_out = false; *et_out = 0;
        } else {
            *q_out = false; *et_out = 0;
        }
    } else if (s->phase == 1) { /* TIMING */
        if (!in_val) {
            s->phase = 0; s->start_tick = 0;
            *q_out = false; *et_out = 0;
        } else {
            uint32_t elapsed = tick - s->start_tick;
            if (elapsed >= pt) {
                s->phase = 2;
                *q_out = true; *et_out = pt;
            } else {
                *q_out = false; *et_out = elapsed;
            }
        }
    } else { /* DONE (phase == 2) */
        if (!in_val) {
            s->phase = 0; s->start_tick = 0;
            *q_out = false; *et_out = 0;
        } else {
            *q_out = true; *et_out = pt;
        }
    }
}

/** TOF step — mirrors LowPLC.TOF.tof_step */
static inline void tof_step(tof_state_t *s, bool in_val, uint32_t pt,
                            uint32_t tick, bool *q_out, uint32_t *et_out) {
    if (s->phase == 0) { /* IDLE */
        if (in_val) {
            s->phase = 0; /* stay idle, but Q=true while IN=true */
            s->prev_in = 1;
            *q_out = true; *et_out = 0;
        } else if (s->prev_in) {
            /* Falling edge — start timing */
            s->phase = 1; s->start_tick = tick;
            s->prev_in = 0;
            *q_out = true; *et_out = 0;
        } else {
            *q_out = false; *et_out = 0;
        }
    } else if (s->phase == 1) { /* TIMING */
        if (in_val) {
            /* IN went true — cancel timing */
            s->phase = 0; s->prev_in = 1;
            *q_out = true; *et_out = 0;
        } else {
            uint32_t elapsed = tick - s->start_tick;
            if (elapsed >= pt) {
                s->phase = 0; s->prev_in = 0;
                *q_out = false; *et_out = pt;
            } else {
                *q_out = true; *et_out = elapsed;
            }
        }
    } else {
        *q_out = false; *et_out = 0;
    }
}

/** TP step — mirrors LowPLC.TP.tp_step */
static inline void tp_step(tp_state_t *s, bool in_val, uint32_t pt,
                           uint32_t tick, bool *q_out, uint32_t *et_out) {
    if (s->phase == 0) { /* IDLE */
        if (in_val && !s->prev_in) {
            /* Rising edge — start pulse */
            s->phase = 1; s->start_tick = tick;
            s->prev_in = 1;
            *q_out = true; *et_out = 0;
        } else {
            s->prev_in = in_val ? 1 : 0;
            *q_out = false; *et_out = 0;
        }
    } else if (s->phase == 1) { /* TIMING */
        uint32_t elapsed = tick - s->start_tick;
        if (elapsed >= pt) {
            s->phase = 0; s->prev_in = in_val ? 1 : 0;
            *q_out = false; *et_out = pt;
        } else {
            s->prev_in = in_val ? 1 : 0;
            *q_out = true; *et_out = elapsed;
        }
    } else {
        *q_out = false; *et_out = 0;
    }
}

/** CTU step — mirrors LowPLC.CTU.ctu_step */
static inline void ctu_step(ctu_state_t *s, bool cu, bool r, int16_t pv,
                            bool *q_out, int16_t *cv_out) {
    if (r) {
        s->cv = 0;
    } else if (cu && !s->prev_cu) {
        if (s->cv < pv) s->cv = add_wrap(s->cv, 1);
    }
    s->prev_cu = cu ? 1 : 0;
    *q_out = s->cv >= pv;
    *cv_out = s->cv;
}

/** CTD step — mirrors LowPLC.CTD.ctd_step */
static inline void ctd_step(ctd_state_t *s, bool cd, bool ld, int16_t pv,
                            bool *q_out, int16_t *cv_out) {
    if (ld) {
        s->cv = pv;
    } else if (cd && !s->prev_cd) {
        if (s->cv > 0) s->cv = sub_wrap(s->cv, 1);
    }
    s->prev_cd = cd ? 1 : 0;
    *q_out = s->cv <= 0;
    *cv_out = s->cv;
}

/** CTUD step — mirrors LowPLC.CTUD.ctud_step */
static inline void ctud_step(ctud_state_t *s, bool cu, bool cd,
                             bool r, bool ld, int16_t pv,
                             bool *qu_out, bool *qd_out, int16_t *cv_out) {
    bool rising_cu = cu && !s->prev_cu;
    bool rising_cd = cd && !s->prev_cd;
    if (r) {
        s->cv = 0;
    } else if (ld) {
        s->cv = pv;
    } else if (rising_cu && rising_cd) {
        /* simultaneous: cancel */
    } else {
        if (rising_cu && s->cv < pv) s->cv = add_wrap(s->cv, 1);
        if (rising_cd && s->cv > 0)  s->cv = sub_wrap(s->cv, 1);
    }
    s->prev_cu = cu ? 1 : 0;
    s->prev_cd = cd ? 1 : 0;
    *qu_out = s->cv >= pv;
    *qd_out = s->cv <= 0;
    *cv_out = s->cv;
}

/** SR step — mirrors LowPLC.SR.sr_step */
static inline void sr_step(sr_state_t *s, bool s1, bool r, bool *q1_out) {
    bool new_q = s1 || (!r && (bool)s->q);
    s->q = new_q ? 1 : 0;
    *q1_out = new_q;
}

/** RS step — mirrors LowPLC.RS.rs_step */
static inline void rs_step(rs_state_t *s, bool s_val, bool r1, bool *q1_out) {
    bool new_q = !r1 && (s_val || (bool)s->q);
    s->q = new_q ? 1 : 0;
    *q1_out = new_q;
}

/** R_TRIG step — mirrors LowPLC.R_TRIG.r_trig_step */
static inline void rtrig_step(rtrig_state_t *s, bool clk, bool *q_out) {
    *q_out = clk && !s->m;
    s->m = clk ? 1 : 0;
}

/** F_TRIG step — mirrors LowPLC.F_TRIG.f_trig_step */
static inline void ftrig_step(ftrig_state_t *s, bool clk, bool *q_out) {
    *q_out = !clk && (bool)s->m;
    s->m = clk ? 1 : 0;
}

/* ── Header Parsing ─────────────────────────────────────────────── */

static inline int parse_header(const uint8_t *code, size_t code_len,
                               tlv_header_t *h) {
    if (code_len < 20) return -1;
    if (code[0] != 0x4C || code[1] != 0x50) return -1;  /* "LP" */
    if (code[2] != 0x01) return -1;  /* version */

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

    /* Bounds checks */
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

/* ── Forward Declarations for Mutual Recursion ──────────────────── */

static void exec_stmts(const uint8_t *code, size_t code_len,
                       size_t off, size_t rem, var_pool_t *p, int gas);
static void exec_if_block(const uint8_t *code, size_t code_len,
                          size_t poff, size_t plen, var_pool_t *p, int gas);
static void exec_elsif_chain(const uint8_t *code, size_t code_len,
                             size_t off, int n, bool *matched,
                             var_pool_t *p, int gas, size_t *after_off);
static void exec_for_block(const uint8_t *code, size_t code_len,
                           size_t poff, size_t plen, var_pool_t *p, int gas);
static void exec_for_iter(const uint8_t *code, size_t code_len,
                          size_t body_off, size_t body_len,
                          unsigned idx_i, int16_t end_v,
                          var_pool_t *p, int iters_left, int gas);

/* ── Atomic Opcode Execution ────────────────────────────────────── */

static void exec_atomic(const uint8_t *code, size_t code_len,
                        size_t off, uint8_t tag, size_t len,
                        var_pool_t *p)
{
    size_t poff = off + 3;
    if (poff + len > code_len) return;

    /* §6.1 Literal Loads */
    if (tag == TAG_LOAD_TRUE && len >= 1)
        set_bool(p, code[poff], true);
    else if (tag == TAG_LOAD_FALSE && len >= 1)
        set_bool(p, code[poff], false);
    else if (tag == TAG_LOAD_INT && len >= 3)
        set_int(p, code[poff], read_i16_le(code, poff+1, code_len));

    /* §6.2 Boolean Operations */
    else if (tag == TAG_BOOL_NOT && len >= 2)
        set_bool(p, code[poff], !get_bool(p, code[poff+1]));
    else if (tag == TAG_BOOL_AND && len >= 3)
        set_bool(p, code[poff], get_bool(p, code[poff+1]) && get_bool(p, code[poff+2]));
    else if (tag == TAG_BOOL_OR && len >= 3)
        set_bool(p, code[poff], get_bool(p, code[poff+1]) || get_bool(p, code[poff+2]));
    else if (tag == TAG_BOOL_XOR && len >= 3)
        set_bool(p, code[poff], get_bool(p, code[poff+1]) != get_bool(p, code[poff+2]));
    else if (tag == TAG_COPY_BOOL && len >= 2)
        set_bool(p, code[poff], get_bool(p, code[poff+1]));

    /* §6.3 Integer Arithmetic */
    else if (tag == TAG_INT_ADD && len >= 3)
        set_int(p, code[poff], add_wrap(get_int(p, code[poff+1]), get_int(p, code[poff+2])));
    else if (tag == TAG_INT_SUB && len >= 3)
        set_int(p, code[poff], sub_wrap(get_int(p, code[poff+1]), get_int(p, code[poff+2])));
    else if (tag == TAG_INT_MUL && len >= 3)
        set_int(p, code[poff], mul_wrap(get_int(p, code[poff+1]), get_int(p, code[poff+2])));
    else if (tag == TAG_COPY_INT && len >= 2)
        set_int(p, code[poff], get_int(p, code[poff+1]));

    /* §6.4 Integer Comparisons */
    else if (tag == TAG_CMP_EQ && len >= 3)
        set_bool(p, code[poff], get_int(p, code[poff+1]) == get_int(p, code[poff+2]));
    else if (tag == TAG_CMP_NE && len >= 3)
        set_bool(p, code[poff], get_int(p, code[poff+1]) != get_int(p, code[poff+2]));
    else if (tag == TAG_CMP_LT && len >= 3)
        set_bool(p, code[poff], get_int(p, code[poff+1]) < get_int(p, code[poff+2]));
    else if (tag == TAG_CMP_GT && len >= 3)
        set_bool(p, code[poff], get_int(p, code[poff+1]) > get_int(p, code[poff+2]));
    else if (tag == TAG_CMP_LE && len >= 3)
        set_bool(p, code[poff], get_int(p, code[poff+1]) <= get_int(p, code[poff+2]));
    else if (tag == TAG_CMP_GE && len >= 3)
        set_bool(p, code[poff], get_int(p, code[poff+1]) >= get_int(p, code[poff+2]));

    /* §6.6 Timer FB Calls */
    else if (tag == TAG_TON_CALL && len >= 7) {
        unsigned idx = code[poff];
        bool q; uint32_t et;
        uint32_t pt = read_u32_le(code, poff+2, code_len);
        if (idx < p->n_ton && p->tick >= p->tons[idx].start_tick) {
            ton_step(&p->tons[idx], get_bool(p, code[poff+1]), pt, p->tick, &q, &et);
            set_bool(p, code[poff+6], q);
        }
    }
    else if (tag == TAG_TOF_CALL && len >= 7) {
        unsigned idx = code[poff];
        bool q; uint32_t et;
        uint32_t pt = read_u32_le(code, poff+2, code_len);
        if (idx < p->n_tof && p->tick >= p->tofs[idx].start_tick) {
            tof_step(&p->tofs[idx], get_bool(p, code[poff+1]), pt, p->tick, &q, &et);
            set_bool(p, code[poff+6], q);
        }
    }
    else if (tag == TAG_TP_CALL && len >= 7) {
        unsigned idx = code[poff];
        bool q; uint32_t et;
        uint32_t pt = read_u32_le(code, poff+2, code_len);
        if (idx < p->n_tp && p->tick >= p->tps[idx].start_tick) {
            tp_step(&p->tps[idx], get_bool(p, code[poff+1]), pt, p->tick, &q, &et);
            set_bool(p, code[poff+6], q);
        }
    }

    /* §6.7 Counter FB Calls */
    else if (tag == TAG_CTU_CALL && len >= 6) {
        unsigned idx = code[poff];
        if (idx < p->n_ctu) {
            bool q; int16_t cv;
            ctu_step(&p->ctus[idx], get_bool(p, code[poff+1]),
                     get_bool(p, code[poff+2]), get_int(p, code[poff+3]),
                     &q, &cv);
            set_bool(p, code[poff+4], q);
            set_int(p, code[poff+5], cv);
        }
    }
    else if (tag == TAG_CTD_CALL && len >= 6) {
        unsigned idx = code[poff];
        if (idx < p->n_ctd) {
            bool q; int16_t cv;
            ctd_step(&p->ctds[idx], get_bool(p, code[poff+1]),
                     get_bool(p, code[poff+2]), get_int(p, code[poff+3]),
                     &q, &cv);
            set_bool(p, code[poff+4], q);
            set_int(p, code[poff+5], cv);
        }
    }
    else if (tag == TAG_CTUD_CALL && len >= 9) {
        unsigned idx = code[poff];
        if (idx < p->n_ctud) {
            bool qu, qd; int16_t cv;
            ctud_step(&p->ctuds[idx],
                      get_bool(p, code[poff+1]), get_bool(p, code[poff+2]),
                      get_bool(p, code[poff+3]), get_bool(p, code[poff+4]),
                      get_int(p, code[poff+5]),
                      &qu, &qd, &cv);
            set_bool(p, code[poff+6], qu);
            set_bool(p, code[poff+7], qd);
            set_int(p, code[poff+8], cv);
        }
    }

    /* §6.8 Bistable and Edge Detector FB Calls */
    else if (tag == TAG_SR_CALL && len >= 4) {
        unsigned idx = code[poff];
        if (idx < p->n_sr) {
            bool q1;
            sr_step(&p->srs[idx], get_bool(p, code[poff+1]),
                    get_bool(p, code[poff+2]), &q1);
            set_bool(p, code[poff+3], q1);
        }
    }
    else if (tag == TAG_RS_CALL && len >= 4) {
        unsigned idx = code[poff];
        if (idx < p->n_rs) {
            bool q1;
            rs_step(&p->rss[idx], get_bool(p, code[poff+1]),
                    get_bool(p, code[poff+2]), &q1);
            set_bool(p, code[poff+3], q1);
        }
    }
    else if (tag == TAG_RTRIG_CALL && len >= 3) {
        unsigned idx = code[poff];
        if (idx < p->n_rtrig) {
            bool q;
            rtrig_step(&p->rtrigs[idx], get_bool(p, code[poff+1]), &q);
            set_bool(p, code[poff+2], q);
        }
    }
    else if (tag == TAG_FTRIG_CALL && len >= 3) {
        unsigned idx = code[poff];
        if (idx < p->n_ftrig) {
            bool q;
            ftrig_step(&p->ftrigs[idx], get_bool(p, code[poff+1]), &q);
            set_bool(p, code[poff+2], q);
        }
    }
    /* else: unknown opcode — skip */
}

/* ── Recursive Interpreter Core ─────────────────────────────────── */

static void exec_stmts(const uint8_t *code, size_t code_len,
                       size_t off, size_t rem, var_pool_t *p, int gas) {
    while (gas > 0 && rem >= 3 && off + 3 <= code_len) {
        uint8_t tag = code[off];
        size_t len = read_u16_le(code, off + 1, code_len);
        if (3 + len > rem || off + 3 + len > code_len) return;

        if (tag == TAG_IF_BLOCK)
            exec_if_block(code, code_len, off + 3, len, p, gas - 1);
        else if (tag == TAG_FOR_BLOCK)
            exec_for_block(code, code_len, off + 3, len, p, gas - 1);
        else
            exec_atomic(code, code_len, off, tag, len, p);

        off += 3 + len;
        rem -= 3 + len;
        gas--;
    }
}

static void exec_if_block(const uint8_t *code, size_t code_len,
                          size_t poff, size_t plen, var_pool_t *p, int gas) {
    /* D5 format: [cond:1][then_count:1][else_count:1]
                  [then_len:2][else_len:2]
                  [then_body:then_len][else_body:else_len]
       ELSIFs are flattened into nested IF_BLOCKs in the else branch. */
    if (gas <= 0 || poff + 7 > code_len) return;

    unsigned cond_b    = code[poff];
    /* then_count and else_count are secondary bounds (P2) — not used at runtime */
    size_t   then_len  = read_u16_le(code, poff + 3, code_len);
    size_t   else_len  = read_u16_le(code, poff + 5, code_len);
    size_t   then_off  = poff + 7;
    size_t   else_off  = then_off + then_len;

    if (then_off + then_len > code_len) return;
    if (else_off + else_len > code_len) return;

    if (get_bool(p, cond_b)) {
        exec_stmts(code, code_len, then_off, then_len, p, gas - 1);
    } else if (else_len > 0) {
        exec_stmts(code, code_len, else_off, else_len, p, gas - 1);
    }
}

static void exec_for_block(const uint8_t *code, size_t code_len,
                           size_t poff, size_t plen, var_pool_t *p, int gas) {
    /* D5 format: [var_idx:1][from_val:2][to_val:2]
                  [body_count:1][body_len:2][body:body_len]
       body_count is a secondary bound (P2) — not used at runtime. */
    if (gas <= 0 || poff + 8 > code_len) return;

    unsigned idx_i  = code[poff];
    int16_t start_v = read_i16_le(code, poff + 1, code_len);
    int16_t end_v   = read_i16_le(code, poff + 3, code_len);
    /* body_count at poff + 5 — secondary bound, skip */
    size_t body_len = read_u16_le(code, poff + 6, code_len);
    size_t body_off = poff + 8;
    if (body_off + body_len > code_len) return;

    int sv = (int)start_v, ev = (int)end_v;
    int iters = (ev >= sv) ? ev - sv + 1 : 0;
    set_int(p, idx_i, start_v);
    exec_for_iter(code, code_len, body_off, body_len, idx_i, end_v, p,
                  iters, gas - 1);
}

static void exec_for_iter(const uint8_t *code, size_t code_len,
                          size_t body_off, size_t body_len,
                          unsigned idx_i, int16_t end_v,
                          var_pool_t *p, int iters_left, int gas) {
    while (gas > 0 && iters_left > 0) {
        exec_stmts(code, code_len, body_off, body_len, p, gas - 1);
        int16_t cur = get_int(p, idx_i);
        int16_t next = add_wrap(cur, 1);
        set_int(p, idx_i, next);
        iters_left--;
        gas--;
    }
}

/* ── Entry Points ───────────────────────────────────────────────── */

/** Execute the INIT section (called once). */
static inline void exec_init(const uint8_t *code, size_t code_len,
                             const tlv_header_t *h, var_pool_t *p, int gas) {
    exec_stmts(code, code_len, 20, h->init_len, p, gas);
}

/** Execute the BODY section (called each scan cycle). */
static inline void exec_scan(const uint8_t *code, size_t code_len,
                             const tlv_header_t *h, var_pool_t *p, int gas) {
    exec_stmts(code, code_len, 20 + h->init_len, h->body_len, p, gas);
}

#endif /* LOWPLC_INTERP_H */
